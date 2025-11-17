/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete_node.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 19:17:49 by yocto             #+#    #+#             */
/*   Updated: 2025/11/17 19:23:24 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	free_arg_node(t_arg *node)
{
	if (node->value)
		free(node->value);
	free(node);
}

t_arg	*find_prev_node(t_arg *head, t_arg *target, t_arg **prev)
{
	t_arg	*tmp;

	*prev = NULL;
	tmp = head;
	while (tmp && tmp != target)
	{
		*prev = tmp;
		tmp = tmp->next;
	}
	return (tmp);
}

void	delete_arg_node(t_arg **head, t_arg *target)
{
	t_arg	*tmp;
	t_arg	*prev;

	if (!head || !*head || !target)
		return ;
	if (*head == target)
	{
		*head = (*head)->next;
		free_arg_node(target);
		return ;
	}
	tmp = find_prev_node(*head, target, &prev);
	if (!tmp)
		return ;
	prev->next = tmp->next;
	free_arg_node(tmp);
}