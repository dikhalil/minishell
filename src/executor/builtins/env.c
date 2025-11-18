/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:21:42 by yocto             #+#    #+#             */
/*   Updated: 2025/11/18 17:32:44 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	env_builtin(t_data *data, t_env *env , t_arg *args)
{
	if(args -> next)
		return;
	while (env)
	{
		if (env->value)
		{
			write(STDOUT_FILENO, env->key, ft_strlen(env->key));
			write(STDOUT_FILENO, "=", 1);
			write(STDOUT_FILENO, env->value, ft_strlen(env->value));
			write(STDOUT_FILENO, "\n", 1);
		}
		env = env->next;
	}
	data->last_exit = 0;
}
