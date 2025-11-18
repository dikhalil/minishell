/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:06:31 by yocto             #+#    #+#             */
/*   Updated: 2025/11/18 19:21:02 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_env_node(t_env *node)
{
	free(node->key);
	free(node->value);
	free(node);
}

static void	remove_env_var(t_data *data, t_env *prev, t_env *current)
{
	if (prev)
		prev->next = current->next;
	else
		data->env = current->next;
	free_env_node(current);
}

static void	unset_single_var(t_data *data, char *var_name)
{
	t_env	*current;
	t_env	*prev;

	current = data->env;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->key, var_name) == 0)
		{
			remove_env_var(data, prev, current);
			break ;
		}
		prev = current;
		current = current->next;
	}
}

void	unset_builtin(t_data *data, t_arg *args)
{
	while (args)
	{
		unset_single_var(data, args->value);
		args = args->next;
	}
	data->last_exit = 0;
}
