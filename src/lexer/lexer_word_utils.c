/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 01:20:24 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/19 21:54:49 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

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

static void	process_word_part(t_data *data, char *str, int *idx,
		t_word_state *state)
{
	char	*part;

	part = extract_word(data, str, idx, &state->quote_type);
	if (!part)
		return ;
	if (state->quote_type != SINGLE_QUOTE && ft_strchr(part, '$'))
	{
		expand_str(data, &part);
		state->expanded = 1;
	}
	if (part)
		handle_part(data, part, &state->word, state->quote_type);
}

int	add_word(t_data *data, char *str)
{
	int				idx;
	t_word_state	state;

	idx = 0;
	state.word = NULL;
	state.expanded = 0;
	while (str[idx] && !is_space(str[idx]) && !is_redir(str[idx]))
		process_word_part(data, str, &idx, &state);
	add_final_token(data, state.word, state.quote_type, state.expanded);
	return (idx);
}
