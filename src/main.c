/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:41:49 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/14 02:39:34 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

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
        if (!token || !*token)
        {
            free(command_line);
            continue ;
        }
        parser(&cmd, &token, &last_exit);
        if (!cmd)
        {
            token_clear(&token, del);
            free(command_line);
            continue ;
        }
        token_clear(&token, del);
        cmd_clear(&cmd, del);
        free(command_line);
    }
    return (0);
}
