/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 16:39:30 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/15 15:15:31 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static int handle_token(t_cmd **cmds, t_token **tokens, t_cmd **current_cmd, t_token **current_token)
{
    if ((*current_token)->type == T_WORD)
        handle_word(cmds, tokens, current_cmd, *current_token);
    else if (is_redirection((*current_token)->type))
    {
        if (!handle_redir(cmds, tokens, current_cmd, current_token))
        {
            *current_token = (*current_token)->next;
            return (0);
        }
    }
    else if ((*current_token)->type == T_PIPE)
        return (handle_pipe(cmds, tokens, current_cmd, *current_token));
    return (1);
}

void parser(t_cmd **cmds, t_token **tokens, int *last_exit)
{
    t_cmd *current_cmd;
    t_token *current_token;
    
    current_cmd = cmd_new(cmds, tokens);
    current_token = *tokens;
    while (current_token)
    {
        if (!handle_token(cmds, tokens, &current_cmd, &current_token))
        {
            parser_error_handling(cmds, tokens, current_token, last_exit);
            return ;   
        }
        current_token = current_token->next;
    }
	cmd_add_back(cmds, current_cmd);
}





