/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 01:20:24 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/19 20:24:40 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void	process_split_array(t_data *data, char **splited, char **word)
{
	int		j;
	t_token	*last;

	j = 0;
	while (splited[j])
	{
		if (splited[j][0] == '\0')
		{
			j++;
			continue ;
		}
		*word = ft_strdup(splited[j]);
		if (!*word)
			free_and_exit(data, splited, word);
		add_token(data, *word, T_WORD, NONE);
		last = token_last(data->tokens);
		if (last)
		{
			last->expanded = 1;
			printf("LEXER: [%s] (type=WORD)\n", last->value);
		}
		free(*word);
		*word = NULL;
		j++;
	}
}

static void	handle_split_part(t_data *data, char *part, char **word)
{
	char	**splited;
	int		i;
	int		has_content;

	splited = ft_split(part, ' ');
	free(part);
	if (!splited)
		free_and_exit(data, NULL, word);
	has_content = 0;
	i = 0;
	while (splited[i])
	{
		if (splited[i][0] != '\0')
			has_content = 1;
		i++;
	}
	if (!has_content)
	{
		free_split(splited);
		return;
	}
	if (*word)
	{
		add_token(data, *word, T_WORD, NONE);
		{
			t_token *last_tok = token_last(data->tokens);
			if (last_tok)
				printf("LEXER: [%s] (type=WORD)\n", last_tok->value);
		}
		free(*word);
		*word = NULL;
	}
	process_split_array(data, splited, word);
	free_split(splited);
}

static void	handle_part(t_data *data, char *part, char **word,
		t_quote_type quote_type)
{
	if (part && quote_type == NONE && ft_strchr(part, ' ')
		&& !(token_last(data->tokens)
			&& is_redirection(token_last(data->tokens)->type)))
		handle_split_part(data, part, word);
	else
	{
		*word = str_join_free(*word, part);
		if (!*word)
			exit_program(data, ERR_MEM);
	}
}

static void	add_final_token(t_data *data, char *word, t_quote_type last_quote,
		int expanded)
{
	t_token	*last;

	if (word)
	{
		add_token(data, word, T_WORD, last_quote);
		last = token_last(data->tokens);
		if (last)
			last->expanded = expanded;
		free(word);
	}
}

int	add_word(t_data *data, char *str)
{
	int			idx;
	char		*word;
	char		*part;
	int			expanded;
	t_quote_type	quote_type;

	idx = 0;
	word = NULL;
	expanded = 0;
	while (str[idx] && !is_space(str[idx]) && !is_redir(str[idx]))
	{
		part = extract_word(data, str, &idx, &quote_type);
		if (!part)
			break ;
		if (quote_type != SINGLE_QUOTE && ft_strchr(part, '$'))
		{
			expand_str(data, &part);
			expanded = 1;
		}
		if (part)
			handle_part(data, part, &word, quote_type);
	}
	add_final_token(data, word, quote_type, expanded);
	return (idx);
}
