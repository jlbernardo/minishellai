/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*expand_single_quote(const char *str, int *i)
{
	char	*result;
	int		start;

	(*i)++;
	start = *i;
	while (str[*i] && str[*i] != '\'')
		(*i)++;
	result = ft_substr(str, start, *i - start);
	if (str[*i] == '\'')
		(*i)++;
	return (result);
}

static char	*expand_double_quote(const char *str, int *i, t_shell *shell)
{
	char	*result;
	char	*tmp;

	result = ft_strdup("");
	(*i)++;
	while (str[*i] && str[*i] != '"')
	{
		if (str[*i] == '$')
		{
			tmp = expand_variable(str, i, shell);
			result = ft_strjoin_free(result, tmp);
			free(tmp);
		}
		else
		{
			result = ft_charjoin(result, str[*i]);
			(*i)++;
		}
	}
	if (str[*i] == '"')
		(*i)++;
	return (result);
}

char	*expand_string(const char *str, t_shell *shell)
{
	char	*result;
	char	*tmp;
	int		i;

	result = ft_strdup("");
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'')
		{
			tmp = expand_single_quote(str, &i);
			result = ft_strjoin_free(result, tmp);
			free(tmp);
		}
		else if (str[i] == '"')
		{
			tmp = expand_double_quote(str, &i, shell);
			result = ft_strjoin_free(result, tmp);
			free(tmp);
		}
		else if (str[i] == '$')
		{
			tmp = expand_variable(str, &i, shell);
			result = ft_strjoin_free(result, tmp);
			free(tmp);
		}
		else
		{
			result = ft_charjoin(result, str[i]);
			i++;
		}
	}
	return (result);
}

void	expand_args(t_cmd *cmd, t_shell *shell)
{
	int		i;
	char	*expanded;
	t_redir	*redir;

	i = 0;
	while (i < cmd->argc)
	{
		expanded = expand_string(cmd->args[i], shell);
		free(cmd->args[i]);
		cmd->args[i] = expanded;
		i++;
	}
	redir = cmd->redirs;
	while (redir)
	{
		expanded = expand_string(redir->file, shell);
		free(redir->file);
		redir->file = expanded;
		redir = redir->next;
	}
}
