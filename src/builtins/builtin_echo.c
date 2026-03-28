/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_n_flag(const char *arg)
{
	int	i;

	if (!arg || arg[0] != '-')
		return (0);
	i = 1;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (i > 1);
}

int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	nl;

	nl = 1;
	i = 1;
	while (i < cmd->argc && is_n_flag(cmd->args[i]))
	{
		nl = 0;
		i++;
	}
	while (i < cmd->argc)
	{
		ft_putstr_fd(cmd->args[i], 1);
		if (i + 1 < cmd->argc)
			ft_putchar_fd(' ', 1);
		i++;
	}
	if (nl)
		ft_putchar_fd('\n', 1);
	return (0);
}
