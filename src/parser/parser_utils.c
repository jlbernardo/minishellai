/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = ft_calloc(1, sizeof(t_cmd));
	return (cmd);
}

void	cmd_add_arg(t_cmd *cmd, const char *arg)
{
	char	**new_args;
	int		i;

	new_args = malloc((cmd->argc + 2) * sizeof(char *));
	if (!new_args)
		return ;
	i = 0;
	while (i < cmd->argc)
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = ft_strdup(arg);
	new_args[i + 1] = NULL;
	free(cmd->args);
	cmd->args = new_args;
	cmd->argc++;
}

static void	free_redirs(t_redir *redir)
{
	t_redir	*tmp;

	while (redir)
	{
		tmp = redir->next;
		free(redir->file);
		free(redir);
		redir = tmp;
	}
}

void	free_cmd(t_cmd *cmd)
{
	t_cmd	*tmp;

	while (cmd)
	{
		tmp = cmd->next;
		free_str_array(cmd->args);
		free_redirs(cmd->redirs);
		free(cmd);
		cmd = tmp;
	}
}

void	free_cmd_group(t_cmd_group *group)
{
	t_cmd_group	*tmp;

	while (group)
	{
		tmp = group->next;
		free_cmd(group->cmds);
		free(group);
		group = tmp;
	}
}
