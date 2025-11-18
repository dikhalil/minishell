/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 18:40:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/18 18:38:16 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_existing_env(t_env *current, char *value, char *key)
{
	free(current->value);
	current->value = value;
	free(key);
}

static void	add_new_env(t_data *data, char *key, char *value)
{
	t_env	*new_node;

	new_node = malloc(sizeof(t_env));
	new_node->key = key;
	new_node->value = value;
	new_node->next = data->env;
	data->env = new_node;
}

void	add_or_update_env(t_data *data, const char *arg)
{
	char	*key;
	char	*value;
	t_env	*current;

	key = strdup_until_char(arg, '=');
	if (ft_strchr(arg, '='))
		value = ft_strdup(ft_strchr(arg, '=') + 1);
	else
		value = NULL;
	current = data->env;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			update_existing_env(current, value, key);
			return ;
		}
		current = current->next;
	}
	add_new_env(data, key, value);
}

void	print_env_sorted(t_env *env)
{
	t_env	*current;

	if (!env)
		return ;
	sort_env_list(env);
	current = env;
	while (current)
	{
		print_env_entry(current);
		current = current->next;
	}
}
