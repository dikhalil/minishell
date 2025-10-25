/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:41:49 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/25 20:36:44 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int g_sig = 0;

// void print_cmds_after_expand(t_data *data)
// {
//     t_cmd *cmd;
//     t_arg *arg;

//     cmd = data->cmds;
//     while (cmd)
//     {
//         arg = cmd->arg;
//         while (arg)
//         {
//             printf("ARG: %s\n", arg->value);
//             arg = arg->next;
//         }
//         cmd = cmd->next;
//     }
// }

int main(int argc, char **argv, char **envp)
{
    t_data data;

    ft_memset(&data, 0, sizeof(t_data));
    init_env(&data, envp);
    data.argv = argv;
    data.argc = argc;
    run_shell(&data);
    env_clear(&data.env);
    return (0);
}
