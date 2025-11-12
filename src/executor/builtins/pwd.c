/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 07:27:23 by yocto             #+#    #+#             */
/*   Updated: 2025/11/12 17:24:16 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void pwd_builtin(t_data *data, t_arg *args)
{
    char	cwd[1024];

    if(args && args->value)
    {
        // write(2, "minishell: pwd: too many arguments\n", 35);
        data->last_exit = 1;
        // return;
    }
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