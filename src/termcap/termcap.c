/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   termcap.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	tc_putchar(int c)
{
	return (write(1, &c, 1));
}

static void	load_caps(t_term *term)
{
	char	*area;
	char	buf[2048];

	area = buf;
	term->ce = tgetstr("ce", &area);
	term->le = tgetstr("le", &area);
	term->nd = tgetstr("nd", &area);
	term->cr = tgetstr("cr", &area);
	term->cl = tgetstr("cl", &area);
	term->cm = tgetstr("cm", &area);
	term->co = tgetnum("co");
	term->li = tgetnum("li");
}

int	init_termcap(t_term *term)
{
	char	*term_type;
	char	tbuf[2048];

	term_type = getenv("TERM");
	if (!term_type)
		term_type = "xterm";
	if (tgetent(tbuf, term_type) <= 0)
	{
		term->init = 0;
		return (-1);
	}
	load_caps(term);
	tcgetattr(STDIN_FILENO, &term->orig);
	term->raw = term->orig;
	term->raw.c_lflag &= ~(ICANON | ECHO | ISIG);
	term->raw.c_cc[VMIN] = 1;
	term->raw.c_cc[VTIME] = 0;
	term->init = 1;
	return (0);
}

void	enable_raw_mode(t_term *term)
{
	if (term->init)
		tcsetattr(STDIN_FILENO, TCSANOW, &term->raw);
}

void	disable_raw_mode(t_term *term)
{
	if (term->init)
		tcsetattr(STDIN_FILENO, TCSANOW, &term->orig);
}

void	tc_clear_line(t_term *term, int prompt_len, int line_len)
{
	int	i;

	(void)prompt_len;
	if (term->cr)
		tputs(term->cr, 1, tc_putchar);
	else
		write(1, "\r", 1);
	if (term->ce)
		tputs(term->ce, 1, tc_putchar);
	else
	{
		i = 0;
		while (i < prompt_len + line_len + 1)
		{
			write(1, " ", 1);
			i++;
		}
		if (term->cr)
			tputs(term->cr, 1, tc_putchar);
		else
			write(1, "\r", 1);
	}
}
