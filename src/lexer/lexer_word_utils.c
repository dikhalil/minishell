/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 14:21:46 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/16 13:38:08 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static t_quote_type	get_quote_type(char c)
{
	if (c == '"')
		return (DOUBLE_QUOTE);
	else if (c == '\'')
		return (SINGLE_QUOTE);
	return (NONE);
}

static int	skip_quoted(char *str, int i, char quote_chr)
{
	while (str[i] && str[i] != quote_chr)
		i++;
	if (!str[i])
		return (-1);
	return (i + 1);
}

static int	skip_unquoted(char *str, int i)
{
	while (str[i] && !is_space(str[i]) && !is_redir(str[i])
		&& get_quote_type(str[i]) == NONE)
		i++;
	return (i);
}

static char	*extract_word(t_data *data, char *str, int *i,
		t_quote_type *quote_type)
{
	int		start;
	char	*part;
	char	quote_chr;

	quote_chr = '\0';
	if (get_quote_type(str[*i]) != NONE)
	{
		*quote_type = get_quote_type(str[*i]);
		quote_chr = str[*i];
		(*i)++;
		start = *i;
		*i = skip_quoted(str, *i, quote_chr);
		if (*i == -1)
			return (NULL);
		part = ft_substr(str, start, *i - start - 1);
	}
	else
	{
		start = *i;
		*i = skip_unquoted(str, *i);
		part = ft_substr(str, start, *i - start);
	}
	if (!part)
		exit_program(data, ERR_MEM);
	return (part);
}

char	*get_word(t_data *data, char *str, int *i, t_quote_type *quote_type)
{
	char	*word;
	char	*part;
	char	*tmp;

	word = NULL;
	while (str[*i] && !is_space(str[*i]) && !is_redir(str[*i]))
	{
		part = extract_word(data, str, i, quote_type);
		if (!part)
			return (NULL);
		if (!word)
			word = ft_strdup(part);
		else
		{
			tmp = word;
			word = ft_strjoin(word, part);
			free(tmp);
		}
		if (!word)
			exit_program(data, ERR_MEM);
		free(part);
	}
	return (word);
}
