/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:41:49 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/16 21:47:16 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

#include <minishell.h>
#include <stdio.h>

void print_cmds_after_expand(t_data *data)
{
    t_cmd *cmd;
    t_arg *arg;
    t_redir *redir;

    cmd = data->cmds;
    while (cmd)
    {
        arg = cmd->arg;
        while (arg)
        {
            printf("ARG: %s\n", arg->value);
            arg = arg->next;
        }
        redir = cmd->redir;
        while (redir)
        {
            printf("FILE: %s\n", redir->file);
            redir = redir->next;
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
    while (TRUE)
    {
        data.command_line = readline(PROMPT);
        if (!data.command_line)
            exit_program(&data, 0);
        if (*data.command_line)
            add_history(data.command_line);
        else
        {
            free(data.command_line);
            continue;
        }
        data.tokens = NULL;
        lexer(&data);
        if (!data.tokens)
            continue ;
        data.cmds = NULL;
        parser(&data);
        if (!data.cmds)
            continue ;
        expand(&data);
        print_cmds_after_expand(&data);
        token_clear(&data.tokens);
        cmd_clear(&data.cmds);
        free(data.command_line);
    }
    env_clear(&data.env);
    return (0);
}
