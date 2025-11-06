/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 14:14:06 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/06 18:16:49 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void handle_commandline(t_data *data)
{
    lexer(data);
    if (!data->tokens)
        return ;
    parser(data);
    if (!data->cmds)
        return ;
    g_sig = 0;
    heredoc(data);
    if (g_sig == SIGINT)
    {
        g_sig = 0;
        free_all(data);
        return ;
    }
    expand(data);
    executor(data);
    free_all(data);
}

static void handle_input(t_data *data)
{
    set_main_signal();
    data->command_line = readline(PROMPT);
	if (g_sig)
		data->last_exit = g_sig + 128;
    if (!data->command_line)
        exit_program(data, data->last_exit);
    if (!ft_strncmp(data->command_line, "exit", 4))
        exit_program(data, data->last_exit);
    if (!*data->command_line)
    {
        free(data->command_line);
        data->command_line = NULL;
        return ;
    }
    add_history(data->command_line);
    handle_commandline(data);
}

void run_shell(t_data *data)
{
    while (TRUE)
    {
        reset_data(data);
        handle_input(data);
    }
}
