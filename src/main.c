/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:41:49 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/14 22:28:19 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

const char *redir_type_name(t_redir *redir)
{
    if (!redir)
        return "UNKNOWN";
    if (redir->type == T_IN_REDIR)
        return "IN";
    if (redir->type == T_OUT_REDIR)
        return "OUT";
    if (redir->type == T_APPEND)
        return "APPEND";
    if (redir->type == T_HEREDOC)
        return "HEREDOC";
    return "UNKNOWN";
}

void print_commands(t_cmd *cmds)
{
    t_cmd *current = cmds;
    while (current)
    {
        printf("Command:\n");
        t_arg *arg = current->arg;
        int i = 0;
        while (arg)
        {
            printf("  Arg[%d]: '%s'\n", i++, arg->value);
            arg = arg->next;
        }

        t_redir *redir = current->redir;
        while (redir)
        {
            printf("  Redir: type=%s, file='%s'\n", redir_type_name(redir), redir->file);
            redir = redir->next;
        }

        printf("-----\n");
        current = current->next;
    }
}


int main(int argc, char **argv, char **envp)
{
    char *command_line;
    int last_exit;
    t_token *token;
    t_cmd *cmd;
    
    (void)argc;
    (void)argv;
    (void)envp;
    while (TRUE)
    {
        token = NULL;
        cmd = NULL;
        last_exit = 0;
        command_line = readline(PROMPT);
        if (!command_line)
            break ;
        if (!*command_line)
        {
            free(command_line);
            continue;
        }
        lexer(command_line, &token, &last_exit);
        if (!token)
        {
            free(command_line);
            continue ;
        }
        parser(&cmd, &token, &last_exit);
        if (!cmd)
        {
            free(command_line);
            continue ;
        }
        print_commands(cmd);
        token_clear(&token);
        cmd_clear(&cmd);
        free(command_line);
    }
    return (0);
}
