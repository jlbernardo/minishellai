/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	child_exec(t_shell *shell, t_cmd *cmd, char **envp)
{
	char	*path;

	if (apply_redirections(cmd->redirs) < 0)
		exit(1);
	if (!cmd->args || !cmd->args[0])
		exit(0);
	if (is_builtin(cmd->args[0]))
		exit(exec_builtin(shell, cmd));
	path = find_executable(cmd->args[0], shell->env);
	if (!path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	execve(path, cmd->args, envp);
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd->args[0], 2);
	ft_putstr_fd(": ", 2);
	ft_putendl_fd(strerror(errno), 2);
	free(path);
	exit(126);
}

static int	count_cmds(t_cmd *cmds)
{
	int	n;

	n = 0;
	while (cmds)
	{
		n++;
		cmds = cmds->next;
	}
	return (n);
}

static void	fork_child(t_shell *shell, t_cmd *cmd, int fds[2], char **envp)
{
	setup_signals_child();
	if (fds[0] != -1)
	{
		dup2(fds[0], STDIN_FILENO);
		close(fds[0]);
	}
	if (fds[1] != -1)
	{
		dup2(fds[1], STDOUT_FILENO);
		close(fds[1]);
	}
	child_exec(shell, cmd, envp);
}

static void	run_pipeline_cmds(t_shell *shell, t_cmd *cmd, pid_t *pids,
		char **envp)
{
	int		pfd[2];
	int		prev_fd;
	int		i;
	int		fds[2];

	prev_fd = -1;
	i = 0;
	while (cmd)
	{
		if (cmd->next && pipe(pfd) < 0)
			return ;
		pids[i] = fork();
		if (pids[i] == 0)
		{
			fds[0] = prev_fd;
			if (cmd->next)
				fds[1] = pfd[1];
			else
				fds[1] = -1;
			if (cmd->next)
				close(pfd[0]);
			fork_child(shell, cmd, fds, envp);
		}
		if (prev_fd != -1)
			close(prev_fd);
		if (cmd->next)
		{
			close(pfd[1]);
			prev_fd = pfd[0];
		}
		cmd = cmd->next;
		i++;
	}
}

void	execute_pipeline(t_shell *shell, t_cmd *cmds)
{
	int		n;
	pid_t	*pids;
	int		status;
	int		i;
	char	**envp;

	n = count_cmds(cmds);
	pids = malloc(n * sizeof(pid_t));
	if (!pids)
		return ;
	envp = env_to_array(shell->env);
	setup_signals_ignore();
	run_pipeline_cmds(shell, cmds, pids, envp);
	i = 0;
	while (i < n)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
	if (WIFEXITED(status))
		shell->last_exit = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		shell->last_exit = 128 + WTERMSIG(status);
	free(pids);
	free_str_array(envp);
	setup_signals_interactive();
}
