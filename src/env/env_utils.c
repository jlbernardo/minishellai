/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_count(t_env *env)
{
	int	count;

	count = 0;
	while (env)
	{
		count++;
		env = env->next;
	}
	return (count);
}

static char	*env_entry_str(t_env *env)
{
	char	*tmp;
	char	*entry;

	tmp = ft_strjoin(env->key, "=");
	if (!tmp)
		return (NULL);
	entry = ft_strjoin(tmp, env->value);
	free(tmp);
	return (entry);
}

char	**env_to_array(t_env *env)
{
	char	**arr;
	int		count;
	int		i;
	t_env	*cur;

	count = env_count(env);
	arr = malloc((count + 1) * sizeof(char *));
	if (!arr)
		return (NULL);
	i = 0;
	cur = env;
	while (cur)
	{
		arr[i] = env_entry_str(cur);
		if (!arr[i])
		{
			free_str_array(arr);
			return (NULL);
		}
		i++;
		cur = cur->next;
	}
	arr[i] = NULL;
	return (arr);
}

void	free_str_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
