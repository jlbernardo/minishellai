/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	read_quoted(const char *input, int i, char quote, char **word)
{
	*word = ft_charjoin(*word, input[i++]);
	while (input[i] && input[i] != quote)
	{
		*word = ft_charjoin(*word, input[i]);
		i++;
	}
	if (input[i] == quote)
	{
		*word = ft_charjoin(*word, input[i]);
		i++;
	}
	return (i);
}

static int	read_word(const char *input, int i, t_token **tokens)
{
	char	*word;

	word = ft_strdup("");
	if (!word)
		return (i);
	while (input[i] && !ft_isspace(input[i]) && !is_special_char(input[i]))
	{
		if (input[i] == '\'' || input[i] == '"')
			i = read_quoted(input, i, input[i], &word);
		else
		{
			word = ft_charjoin(word, input[i]);
			i++;
		}
	}
	token_add_back(tokens, new_token(TOKEN_WORD, word));
	free(word);
	return (i);
}

static int	read_operator(const char *input, int i, t_token **tokens)
{
	if (input[i] == '|')
		token_add_back(tokens, new_token(TOKEN_PIPE, "|"));
	else if (input[i] == ';')
		token_add_back(tokens, new_token(TOKEN_SEMICOLON, ";"));
	else if (input[i] == '<')
		token_add_back(tokens, new_token(TOKEN_REDIR_IN, "<"));
	else if (input[i] == '>' && input[i + 1] == '>')
	{
		token_add_back(tokens, new_token(TOKEN_REDIR_APPEND, ">>"));
		return (i + 2);
	}
	else if (input[i] == '>')
		token_add_back(tokens, new_token(TOKEN_REDIR_OUT, ">"));
	return (i + 1);
}

t_token	*tokenize(const char *input)
{
	t_token	*tokens;
	int		i;

	tokens = NULL;
	i = 0;
	while (input[i])
	{
		i = skip_whitespace(input, i);
		if (!input[i])
			break ;
		if (is_special_char(input[i]))
			i = read_operator(input, i, &tokens);
		else
			i = read_word(input, i, &tokens);
	}
	return (tokens);
}
