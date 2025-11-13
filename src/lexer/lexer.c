/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 15:13:07 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/13 13:22:11 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int	add_token(t_data *data, char *value, t_token_type type,
		t_quote_type quote)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		exit_program(data, ERR_MEM);
	new->value = ft_strdup(value);
	if (!new->value)
		exit_program(data, ERR_MEM);
	new->quote = quote;
	new->type = type;
	new->next = NULL;
	new->expanded = 0;
	token_add_back(&data->tokens, new);
	return (ft_strlen(new->value));
}

static int	add_redir(t_data *data, char chr, char next_chr)
{
    if (chr == '|')
        return (add_token(data, "|", T_PIPE, NONE));
    else if (chr == '<' && next_chr == '<')
        return (add_token(data, "<<", T_HEREDOC, NONE));
    else if (chr == '<')
        return (add_token(data, "<", T_IN_REDIR, NONE));
    else if (chr == '>' && next_chr == '>')
        return (add_token(data, ">>", T_APPEND, NONE));
    else
        return (add_token(data, ">", T_OUT_REDIR, NONE));
}

void	lexer(t_data *data)
{
	int	i;
	int	wordlen;

	if (!data || !data->command_line)
		return ;
	i = 0;
	wordlen = 0;
	while (data->command_line[i])
	{
		if (is_space(data->command_line[i]))
			i++;
		else if (is_redir(data->command_line[i]))
			i += add_redir(data, data->command_line[i], data->command_line[i
					+ 1]);
		else
		{
			wordlen = add_word(data, &data->command_line[i]);
			if (wordlen == -1)
			{
				token_error_handling(data);
				return ;
			}
			i += wordlen;
		}
	}
}
