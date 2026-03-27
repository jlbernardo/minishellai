/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	add_redir(t_cmd *cmd, t_redir_type type, const char *file)
{
	t_redir	*redir;
	t_redir	*cur;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return ;
	redir->type = type;
	redir->file = ft_strdup(file);
	redir->next = NULL;
	if (!cmd->redirs)
		cmd->redirs = redir;
	else
	{
		cur = cmd->redirs;
		while (cur->next)
			cur = cur->next;
		cur->next = redir;
	}
}

static t_token	*parse_redir(t_token *tok, t_cmd *cmd)
{
	t_redir_type	type;

	if (tok->type == TOKEN_REDIR_IN)
		type = REDIR_IN;
	else if (tok->type == TOKEN_REDIR_OUT)
		type = REDIR_OUT;
	else
		type = REDIR_APPEND;
	tok = tok->next;
	if (tok && tok->type == TOKEN_WORD)
	{
		add_redir(cmd, type, tok->value);
		return (tok->next);
	}
	ft_putstr_fd("minishell: syntax error near redirection\n", 2);
	return (tok);
}

static t_token	*parse_command(t_token *tok, t_cmd **cmd_out)
{
	t_cmd	*cmd;

	cmd = new_cmd();
	if (!cmd)
		return (tok);
	while (tok && tok->type != TOKEN_PIPE && tok->type != TOKEN_SEMICOLON)
	{
		if (tok->type == TOKEN_REDIR_IN || tok->type == TOKEN_REDIR_OUT
			|| tok->type == TOKEN_REDIR_APPEND)
			tok = parse_redir(tok, cmd);
		else if (tok->type == TOKEN_WORD)
		{
			cmd_add_arg(cmd, tok->value);
			tok = tok->next;
		}
		else
			tok = tok->next;
	}
	*cmd_out = cmd;
	return (tok);
}

static t_token	*parse_pipeline(t_token *tok, t_cmd_group **grp_out)
{
	t_cmd_group	*grp;
	t_cmd		*cmd;
	t_cmd		*tail;

	grp = ft_calloc(1, sizeof(t_cmd_group));
	if (!grp)
		return (tok);
	tail = NULL;
	while (tok && tok->type != TOKEN_SEMICOLON)
	{
		tok = parse_command(tok, &cmd);
		if (tail)
			tail->next = cmd;
		else
			grp->cmds = cmd;
		tail = cmd;
		if (tok && tok->type == TOKEN_PIPE)
			tok = tok->next;
	}
	*grp_out = grp;
	return (tok);
}

t_cmd_group	*parse(t_token *tokens)
{
	t_cmd_group	*groups;
	t_cmd_group	*grp;
	t_cmd_group	*tail;

	groups = NULL;
	tail = NULL;
	while (tokens)
	{
		parse_pipeline(tokens, &grp);
		tokens = parse_pipeline(tokens, &grp);
		if (tail)
			tail->next = grp;
		else
			groups = grp;
		tail = grp;
		if (tokens && tokens->type == TOKEN_SEMICOLON)
			tokens = tokens->next;
	}
	return (groups);
}
