/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_pwd(t_shell *shell)
{
	char	cwd[4096];

	if (getcwd(cwd, sizeof(cwd)))
	{
		set_env_value(&shell->env, "OLDPWD",
			get_env_value(shell->env, "PWD"));
		set_env_value(&shell->env, "PWD", cwd);
	}
}

int	builtin_cd(t_shell *shell, t_cmd *cmd)
{
	char	*path;

	if (cmd->argc == 1)
	{
		path = get_env_value(shell->env, "HOME");
		if (!path)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (1);
		}
	}
	else
		path = cmd->args[1];
	if (chdir(path) != 0)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		return (1);
	}
	update_pwd(shell);
	return (0);
}
