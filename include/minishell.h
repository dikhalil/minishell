/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:11:01 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/12 23:43:20 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#define TRUE 1
#define ERR_MEM 1
#define ERR_QUOTE 2
#define PROMPT "minishell$ "

typedef enum e_token_type
{
    T_WORD,
    T_PIPE,
    T_IN_REDIR,
    T_OUT_REDIR,
    T_APPEND,
    T_HEREDOC
}   t_token_type;

typedef enum e_quote_type
{
    NONE,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
}   t_quote_type;

typedef struct s_token
{
    char *value;
    t_token_type type;
    t_quote_type quote;
    struct s_token *next;
}   t_token;

#endif

