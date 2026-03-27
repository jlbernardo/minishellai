/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	expand_pipeline(t_shell *shell, t_cmd *cmds)
{
	while (cmds)
	{
		expand_args(cmds, shell);
		cmds = cmds->next;
	}
}

static void	exec_single_builtin(t_shell *shell, t_cmd *cmd)
{
	int	saved_stdin;
	int	saved_stdout;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (apply_redirections(cmd->redirs) < 0)
	{
		shell->last_exit = 1;
		dup2(saved_stdin, STDIN_FILENO);
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdin);
		close(saved_stdout);
		return ;
	}
	shell->last_exit = exec_builtin(shell, cmd);
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
}

static void	execute_cmd_group(t_shell *shell, t_cmd_group *group)
{
	t_cmd	*cmd;

	cmd = group->cmds;
	if (!cmd)
		return ;
	expand_pipeline(shell, cmd);
	if (!cmd->next && cmd->args && cmd->args[0] && is_builtin(cmd->args[0]))
	{
		exec_single_builtin(shell, cmd);
		return ;
	}
	execute_pipeline(shell, cmd);
}

void	execute_groups(t_shell *shell, t_cmd_group *groups)
{
	while (groups)
	{
		execute_cmd_group(shell, groups);
		groups = groups->next;
	}
}
