/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:41:49 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/12 20:55:38 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int main(int argc, char **argv, char **envp)
{
    char *command_line;
    int last_exit;
    t_token *token;
    
    token = NULL;
    while (TRUE)
    {
        last_exit = 0;
        command_line = readline(PROMPT);
        if (!command_line)
            break ;
        if (!*command_line)
        {
            free(command_line);
            continue;
        }
        if (token)
            token_clear(&token, del);
        lexer(command_line, &token, &last_exit);
        if (!token)
        {
            free(command_line);
            continue ;
        }
    }
}
