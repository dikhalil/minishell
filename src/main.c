/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:41:49 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/20 16:16:28 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

volatile sig_atomic_t g_sig = 0;

void print_cmds_after_expand(t_data *data)
{
    t_cmd *cmd;
    t_arg *arg;

    cmd = data->cmds;
    while (cmd)
    {
        arg = cmd->arg;
        while (arg)
        {
            printf("ARG: %s\n", arg->value);
            arg = arg->next;
        }
        cmd = cmd->next;
    }
}

int main(int argc, char **argv, char **envp)
{
    t_data data;

    ft_memset(&data, 0, sizeof(t_data));
    init_env(&data, envp);
    data.argv = argv;
    data.argc = argc;
    data.last_exit = 0;
    while (TRUE)
    {
 
        set_prompt_signal();
        data.command_line = readline(PROMPT);
        if (!data.command_line)
            exit_program(&data, data.last_exit);
        if (!ft_strcmp(data.command_line, "exit"))
            exit_program(&data, data.last_exit);
        if (*data.command_line)
            add_history(data.command_line);
        else
        {
            free(data.command_line);
            continue ;
        }
        data.tokens = NULL;
        lexer(&data);
        if (!data.tokens)
            continue ;
        data.cmds = NULL;
        parser(&data);
        if (!data.cmds)
            continue ;
        g_sig = 0;
        heredoc(&data);
        if (g_sig == SIGINT)
        {
            g_sig = 0;
            free_all(&data);
            continue ;
        }
        expand(&data);
        print_cmds_after_expand(&data);
        // executor(&data);
        free_all(&data);
    }
    env_clear(&data.env);
    return (0);
}