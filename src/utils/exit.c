/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 20:57:51 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/16 14:15:39 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void exit_program(t_data *data, int status)
{
    if (data)
    {
        if (data->command_line)
            free(data->command_line);
        if (data->env)
            env_clear(&data->env);
        if (data->tokens)
            token_clear(&data->tokens);
        if (data->cmds)
            cmd_clear(&data->cmds);
    }
    exit(status);
}
