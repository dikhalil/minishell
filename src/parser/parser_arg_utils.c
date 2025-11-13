/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_arg_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 14:37:41 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/13 11:58:27 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

t_arg	*arg_new(t_data *data, t_cmd **current_cmd, t_token *current_token)
{
	t_arg	*new;

	new = ft_calloc(1, sizeof(t_arg));
	if (!new)
	{
		cmd_clear(current_cmd);
		exit_program(data, ERR_MEM);
	}
	new->quote = current_token->quote;
	new->value = ft_strdup(current_token->value);
	new->expanded = current_token->expanded;
	if (!new->value)
	{
		cmd_clear(current_cmd);
		free(new);
		exit_program(data, ERR_MEM);
	}
	return (new);
}

t_arg	*arg_last(t_arg *head)
{
	if (!head)
		return (NULL);
	while (head->next)
		head = head->next;
	return (head);
}

void	arg_add_back(t_arg **head, t_arg *new)
{
	t_arg	*last;

	if (!*head)
	{
		*head = new;
		return ;
	}
	last = arg_last(*head);
	last->next = new;
}

void	handle_word(t_data *data, t_cmd **current_cmd, t_token *current_token)
{
	t_arg	*arg;

	arg = arg_new(data, current_cmd, current_token);
	arg_add_back(&(*current_cmd)->arg, arg);
}
