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

static void	load_caps(t_term *term)
{
	char		*area;
	static char	buf[2048];

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
