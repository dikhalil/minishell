/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 22:41:21 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/20 10:54:02 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void	handle_env_alloc_error(t_data *data, char *key, t_env *new)
{
	free(key);
	if (new)
		free(new);
	exit_program(data, ERR_MEM);
}

static void	handle_env_strdup_error(t_data *data, char *key, t_env *new)
{
	if (new->key)
		free(new->key);
	free(new);
	free(key);
	exit_program(data, ERR_MEM);
}

t_env	*env_new(t_data *data, char *key, char *value)
{
	t_env	*new;

	if (!key)
		exit_program(data, ERR_MEM);
	new = malloc(sizeof(t_env));
	if (!new)
		handle_env_alloc_error(data, key, NULL);
	new->key = ft_strdup(key);
	if (!new->key)
		handle_env_alloc_error(data, key, new);
	new->value = ft_strdup(value);
	if (!new->value)
		handle_env_strdup_error(data, key, new);
	new->next = NULL;
	return (new);
}

void	init_env(t_data *data, char **envp)
{
	int		i;
	char	*equal;
	char	*tmp;

	i = 0;
	while (envp[i])
	{
		tmp = ft_strdup(envp[i]);
		if (!tmp)
			exit_program(data, ERR_MEM);
		equal = ft_strchr(tmp, '=');
		if (equal)
		{
			*equal = '\0';
			env_add_back(&data->env, env_new(data, tmp, equal + 1));
		}
		else
			env_add_back(&data->env, env_new(data, tmp, ""));
		free(tmp);
		i++;
	}
	increment_shlvl(data);
}
