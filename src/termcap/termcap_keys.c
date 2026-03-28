/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   termcap_keys.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	line_init(t_line *line)
{
	line->cap = LINE_INIT_CAP;
	line->buf = ft_calloc(line->cap, 1);
	line->len = 0;
	line->pos = 0;
}

static void	line_grow(t_line *line)
{
	char	*new_buf;

	if (line->len + 2 < line->cap)
		return ;
	line->cap *= 2;
	new_buf = ft_calloc(line->cap, 1);
	if (!new_buf)
		return ;
	ft_memcpy(new_buf, line->buf, line->len);
	free(line->buf);
	line->buf = new_buf;
}

static void	refresh_line(t_line *line, t_term *term)
{
	int	plen;

	plen = ft_strlen(PROMPT);
	write(1, "\r", 1);
	if (term->ce)
	{
		write(1, "\033[K", 3);
	}
	else
	{
		write(1, "\033[2K", 4);
		write(1, "\r", 1);
	}
	write(1, PROMPT, plen);
	write(1, line->buf, line->len);
	if (line->pos < line->len)
	{
		printf("\033[%dD", line->len - line->pos);
		fflush(stdout);
	}
}

static void	handle_backspace(t_line *line, t_term *term)
{
	int	i;

	if (line->pos <= 0)
		return ;
	i = line->pos - 1;
	while (i < line->len - 1)
	{
		line->buf[i] = line->buf[i + 1];
		i++;
	}
	line->len--;
	line->pos--;
	line->buf[line->len] = '\0';
	refresh_line(line, term);
}

static void	insert_char(t_line *line, char c, t_term *term)
{
	int	i;

	line_grow(line);
	i = line->len;
	while (i > line->pos)
	{
		line->buf[i] = line->buf[i - 1];
		i--;
	}
	line->buf[line->pos] = c;
	line->pos++;
	line->len++;
	line->buf[line->len] = '\0';
	refresh_line(line, term);
}

static void	handle_arrow(t_line *line, t_term *term, t_shell *shell)
{
	char	seq;

	if (read(STDIN_FILENO, &seq, 1) <= 0)
		return ;
	if (seq == 'D' && line->pos > 0)
	{
		line->pos--;
		refresh_line(line, term);
	}
	else if (seq == 'C' && line->pos < line->len)
	{
		line->pos++;
		refresh_line(line, term);
	}
	else if (seq == 'A')
	{
		if (!shell->hist_cur && shell->hist_tail)
			shell->hist_cur = shell->hist_tail;
		else if (shell->hist_cur && shell->hist_cur->prev)
			shell->hist_cur = shell->hist_cur->prev;
		else
			return ;
		ft_strlcpy(line->buf, shell->hist_cur->line, line->cap);
		line->len = ft_strlen(line->buf);
		line->pos = line->len;
		refresh_line(line, term);
	}
	else if (seq == 'B')
	{
		if (shell->hist_cur && shell->hist_cur->next)
		{
			shell->hist_cur = shell->hist_cur->next;
			ft_strlcpy(line->buf, shell->hist_cur->line, line->cap);
		}
		else
		{
			shell->hist_cur = NULL;
			line->buf[0] = '\0';
		}
		line->len = ft_strlen(line->buf);
		line->pos = line->len;
		refresh_line(line, term);
	}
}

static void	handle_escape(t_line *line, t_term *term, t_shell *shell)
{
	char	seq;

	if (read(STDIN_FILENO, &seq, 1) <= 0)
		return ;
	if (seq == '[')
		handle_arrow(line, term, shell);
}

static char	*read_line_raw(t_shell *shell)
{
	t_line	line;
	char	c;

	line_init(&line);
	if (!line.buf)
		return (NULL);
	write(1, PROMPT, ft_strlen(PROMPT));
	shell->hist_cur = NULL;
	while (1)
	{
		if (read(STDIN_FILENO, &c, 1) <= 0)
		{
			free(line.buf);
			return (NULL);
		}
		if (c == '\n' || c == '\r')
		{
			write(1, "\n", 1);
			line.buf[line.len] = '\0';
			return (line.buf);
		}
		else if (c == 4)
		{
			if (line.len == 0)
			{
				free(line.buf);
				return (NULL);
			}
		}
		else if (c == 127 || c == 8)
			handle_backspace(&line, &shell->term);
		else if (c == 27)
			handle_escape(&line, &shell->term, shell);
		else if (c == 3)
		{
			line.buf[0] = '\0';
			line.len = 0;
			line.pos = 0;
			write(1, "\n", 1);
			shell->last_exit = 130;
			refresh_line(&line, &shell->term);
		}
		else if (c >= 32)
			insert_char(&line, c, &shell->term);
	}
}

static char	*read_line_basic(void)
{
	char	*line;
	char	buf[2];
	int		ret;

	line = ft_strdup("");
	write(1, PROMPT, ft_strlen(PROMPT));
	while (1)
	{
		ret = read(STDIN_FILENO, buf, 1);
		if (ret <= 0)
		{
			if (ft_strlen(line) == 0)
			{
				free(line);
				return (NULL);
			}
			return (line);
		}
		if (buf[0] == '\n')
			return (line);
		buf[1] = '\0';
		line = ft_strjoin_free(line, buf);
	}
}

char	*read_line_tc(t_shell *shell)
{
	char	*line;

	if (shell->interactive && shell->term.init)
	{
		enable_raw_mode(&shell->term);
		line = read_line_raw(shell);
		disable_raw_mode(&shell->term);
		return (line);
	}
	return (read_line_basic());
}
