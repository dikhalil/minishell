/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 15:43:13 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/12 22:07:54 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

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

void	token_clear(t_token **tokens)
{
	t_token	*tmp;

	if (!tokens)
		return ;
	while (*tokens)
	{
		free((*tokens)->value);
		tmp = *tokens;
		*tokens = (*tokens)->next;
		free(tmp);
	}
	*tokens = NULL;
}

void	token_error_handling(t_data *data)
{
	ft_putendl_fd("minishell: syntax error unclosed quotes", 2);
	data->last_exit = 2;
	free_all(data);
}
