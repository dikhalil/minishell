/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:21:42 by yocto             #+#    #+#             */
/*   Updated: 2025/11/03 17:22:06 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void env_builtin(t_env *env)
{
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
}