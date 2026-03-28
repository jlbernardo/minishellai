/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <signal.h>
# include <dirent.h>
# include <string.h>
# include <errno.h>
# include <termios.h>
# include <term.h>
# include "../libft/libft.h"

# define PROMPT "minishell$ "
# define HIST_MAX 500
# define LINE_INIT_CAP 256

extern int	g_signal;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_SEMICOLON,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND
}	t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char			**args;
	int				argc;
	t_redir			*redirs;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_cmd_group
{
	t_cmd				*cmds;
	struct s_cmd_group	*next;
}	t_cmd_group;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_history
{
	char				*line;
	struct s_history	*next;
	struct s_history	*prev;
}	t_history;

typedef struct s_term
{
	struct termios	orig;
	struct termios	raw;
	char			*cm;
	char			*ce;
	char			*le;
	char			*nd;
	char			*cr;
	char			*cl;
	int				co;
	int				li;
	int				init;
}	t_term;

typedef struct s_line
{
	char	*buf;
	int		len;
	int		pos;
	int		cap;
}	t_line;

typedef struct s_shell
{
	t_env		*env;
	int			last_exit;
	t_history	*history;
	t_history	*hist_tail;
	t_history	*hist_cur;
	int			hist_count;
	t_term		term;
	int			interactive;
}	t_shell;

/* main.c */
void			process_line(t_shell *shell, char *line);

/* env/env.c */
t_env			*init_env(char **envp);
char			*get_env_value(t_env *env, const char *key);
void			set_env_value(t_env **env, const char *key, const char *value);
void			unset_env_value(t_env **env, const char *key);
void			free_env(t_env *env);

/* env/env_utils.c */
char			**env_to_array(t_env *env);
void			free_str_array(char **arr);
int				env_count(t_env *env);

/* lexer/lexer.c */
t_token			*tokenize(const char *input);

/* lexer/lexer_utils.c */
int				is_special_char(char c);
int				skip_whitespace(const char *s, int i);

/* lexer/token.c */
t_token			*new_token(t_token_type type, const char *value);
void			token_add_back(t_token **lst, t_token *new_tok);
void			free_tokens(t_token *tokens);

/* parser/parser.c */
t_cmd_group		*parse(t_token *tokens);

/* parser/parser_utils.c */
t_cmd			*new_cmd(void);
void			cmd_add_arg(t_cmd *cmd, const char *arg);
void			free_cmd(t_cmd *cmd);
void			free_cmd_group(t_cmd_group *group);

/* expander/expander.c */
char			*expand_string(const char *str, t_shell *shell);
void			expand_args(t_cmd *cmd, t_shell *shell);

/* expander/expander_utils.c */
char			*expand_variable(const char *str, int *i, t_shell *shell);

/* executor/executor.c */
void			execute_groups(t_shell *shell, t_cmd_group *groups);

/* executor/executor_utils.c */
char			*find_executable(const char *cmd, t_env *env);
int				is_builtin(const char *cmd);

/* executor/redirections.c */
int				apply_redirections(t_redir *redirs);

/* executor/pipe.c */
void			execute_pipeline(t_shell *shell, t_cmd *cmds);

/* builtins/builtins.c */
int				exec_builtin(t_shell *shell, t_cmd *cmd);

/* builtins/builtin_echo.c */
int				builtin_echo(t_cmd *cmd);

/* builtins/builtin_cd.c */
int				builtin_cd(t_shell *shell, t_cmd *cmd);

/* builtins/builtin_pwd.c */
int				builtin_pwd(void);

/* builtins/builtin_export.c */
int				builtin_export(t_shell *shell, t_cmd *cmd);

/* builtins/builtin_unset.c */
int				builtin_unset(t_shell *shell, t_cmd *cmd);

/* builtins/builtin_env.c */
int				builtin_env(t_shell *shell);

/* builtins/builtin_exit.c */
int				builtin_exit(t_shell *shell, t_cmd *cmd);

/* signals/signals.c */
void			setup_signals_interactive(void);
void			setup_signals_child(void);
void			setup_signals_ignore(void);

/* termcap/termcap.c */
int				init_termcap(t_term *term);
void			enable_raw_mode(t_term *term);
void			disable_raw_mode(t_term *term);

/* termcap/termcap_keys.c */
char			*read_line_tc(t_shell *shell);

/* termcap/history.c */
void			add_history_entry(t_shell *shell, const char *line);
void			free_history(t_history *history);

#endif
