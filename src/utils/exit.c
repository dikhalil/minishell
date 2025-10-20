/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 20:57:51 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/19 16:46:39 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>
void free_all(t_data *data)
{
    if (data)
    {
        if (data->command_line)
            free(data->command_line);
        if (data->tokens)
            token_clear(&data->tokens);
        if (data->cmds)
            cmd_clear(&data->cmds);
    }   
}
void exit_program(t_data *data, int status)
{
    free_all(data);
    if (data->env)
        env_clear(&data->env);
    rl_clear_history();
    ft_putendl_fd("exit", 0);
    exit(status);
}
