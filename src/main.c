/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	init_shell(t_shell *shell, char **envp)
{
	ft_bzero(shell, sizeof(t_shell));
	shell->env = init_env(envp);
	shell->interactive = isatty(STDIN_FILENO);
	if (shell->interactive)
		init_termcap(&shell->term);
}

static void	cleanup_shell(t_shell *shell)
{
	if (shell->interactive && shell->term.init)
		disable_raw_mode(&shell->term);
	free_env(shell->env);
	free_history(shell->history);
}

void	process_line(t_shell *shell, char *line)
{
	t_token		*tokens;
	t_cmd_group	*groups;

	tokens = tokenize(line);
	if (!tokens)
		return ;
	groups = parse(tokens);
	free_tokens(tokens);
	if (!groups)
		return ;
	execute_groups(shell, groups);
	free_cmd_group(groups);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	char	*line;

	(void)argc;
	(void)argv;
	init_shell(&shell, envp);
	setup_signals_interactive();
	while (1)
	{
		line = read_line_tc(&shell);
		if (!line)
		{
			if (shell.interactive)
				write(1, "exit\n", 5);
			break ;
		}
		if (*line)
		{
			add_history_entry(&shell, line);
			process_line(&shell, line);
		}
		free(line);
	}
	cleanup_shell(&shell);
	return (shell.last_exit);
}
