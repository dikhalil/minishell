/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 07:27:23 by yocto             #+#    #+#             */
/*   Updated: 2025/11/18 17:40:50 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void pwd_builtin(t_data *data, t_arg *args)
{
    char	cwd[1024];

    if(args && args->value)
        data->last_exit = 1;
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        ft_putendl_fd(cwd, STDOUT_FILENO);
        data->last_exit = 0;
    }
    else
    {
        perror("minishell: pwd");
        data->last_exit = 1;
    }
}