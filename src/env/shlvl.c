/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shlvl.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 14:29:35 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/12 08:38:48 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static t_env	*env_get(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env);
		env = env->next;
	}
	return (NULL);
}

void	increment_shlvl(t_data *data)
{
	t_env	*shlvl;
	int		lvl;
	char	*lvl_value;

	shlvl = env_get(data->env, "SHLVL");
	lvl = 1;
	if (shlvl && shlvl->value)
		lvl = ft_atoi(shlvl->value) + 1;
	if (lvl >= 1000)
	{
		ft_putendl_fd("minishell:  warning: shell level (1000) too high, resetting to 1", 2);
		lvl = 1;
	}
	else if (lvl < 0)
		lvl = 0;
	lvl_value = ft_itoa(lvl);
	if (!lvl_value)
		exit_program(data, ERR_MEM);
	if (shlvl)
	{
		free(shlvl->value);
		shlvl->value = lvl_value;
	}
	else
	{
		env_add_back(&data->env, env_new(data, ft_strdup("SHLVL"), lvl_value));
		free(lvl_value);
	}
}
