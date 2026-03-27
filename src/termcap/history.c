/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_history_entry(t_shell *shell, const char *line)
{
	t_history	*entry;

	if (!line || !*line)
		return ;
	entry = malloc(sizeof(t_history));
	if (!entry)
		return ;
	entry->line = ft_strdup(line);
	entry->next = NULL;
	entry->prev = shell->hist_tail;
	if (shell->hist_tail)
		shell->hist_tail->next = entry;
	else
		shell->history = entry;
	shell->hist_tail = entry;
	shell->hist_count++;
}

void	free_history(t_history *history)
{
	t_history	*tmp;

	while (history)
	{
		tmp = history->next;
		free(history->line);
		free(history);
		history = tmp;
	}
}
