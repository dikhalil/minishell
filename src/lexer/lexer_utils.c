/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 15:43:13 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/14 16:53:56 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int is_space(char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}

int is_redir(char c)
{
    return (c == '>' || c == '<' || c == '|');
}

t_token	*token_last(t_token *head)
{
	if (!head)
		return (NULL);
	while (head->next)
		head = head->next;
	return (head);
}

void	token_add_back(t_token **head, t_token *new)
{
	t_token	*last;

	if (!head || !new)
		return ;
	if (!*head)
	{
		*head = new;
		return ;
	}
	last = token_last(*head);
	last->next = new;
}

t_quote_type get_quote_type(char c)
{
    if (c == '"')
        return DOUBLE_QUOTE;
    else if (c == '\'')
        return SINGLE_QUOTE;
    return NONE;
}

