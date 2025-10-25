/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_char_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:53:39 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/20 21:31:38 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int is_space(char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}

int is_redir(char c)
{
    return (c == '>' || c == '<' || c == '|');
}

int is_redirection(t_token_type type)
{
    return (type == T_IN_REDIR || type == T_OUT_REDIR ||
            type == T_APPEND || type == T_HEREDOC);
}