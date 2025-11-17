/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 01:20:24 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/18 01:36:11 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void	free_and_exit(t_data *data, char **splited, char **word)
{
    if (splited)
	    free_split(splited);
	if (*word)
		free(*word);
	exit_program(data, ERR_MEM);
}

static void	process_split_array(t_data *data, char **splited, char **word)
{
    int		j;
    t_token *last;

    j = -1;
    if (!splited[0])
	{
		*word = str_join_free(*word, ft_strdup(""));
		if (!*word)
			free_and_exit(data, splited, NULL);
	}
    while (splited[++j])
	{            
		*word = ft_strdup(splited[j]);
		if (!*word)
			free_and_exit(data, splited, word);
		add_token(data, *word, T_WORD, NONE);
		last = token_last(data->tokens);
		last->expanded = 1;		
        free(*word);
		*word = NULL;
	}
}

static void	handle_part(t_data *data, char *part, char **word, t_quote_type quote_type)
{
    char	**splited;

    if (part && quote_type == NONE && ft_strchr(part, ' ')
        && !(token_last(data->tokens) && is_redirection(token_last(data->tokens)->type)))
    {
        splited = ft_split(part, ' ');
        free(part);
        part = NULL;
        if (!splited)
            free_and_exit(data, NULL, word);
        if (*word && splited[0])
        {
		    add_token(data, *word, T_WORD, NONE);
            free(*word);
            *word = NULL;
        }
        process_split_array(data, splited, word);
        free_split(splited);
    }
    else
    {
        *word = str_join_free(*word, part);
        if (!*word)
            exit_program(data, ERR_MEM);
    }
}

static void	add_final_token(t_data *data, char *word,
	t_quote_type last_quote, int expanded)
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
    int				idx;
    char			*word;
    char			*part;
    int				expanded;
    t_quote_type	quote_type;

    idx = 0;
    word = NULL;
    expanded = 0;
    while (str[idx] && !is_space(str[idx]) && !is_redir(str[idx]))
    {
        part = extract_word(data, str, &idx, &quote_type);
        if (!part)
            break;
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
