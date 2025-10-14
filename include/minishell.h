/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:11:01 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/14 22:20:23 by dikhalil         ###   ########.fr       */
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
#include "../libft/libft.h"
#define TRUE 1
#define FALSE 0
#define ERR_MEM 1
// #define ERR_QUOTE 2

#define PROMPT "minishell$ "

typedef enum e_token_type
{
    T_WORD,
    T_PIPE,
    T_IN_REDIR,
    T_OUT_REDIR,
    T_APPEND,
    T_HEREDOC,
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

typedef struct s_arg
{
    char            *value;
    struct s_arg    *next;
}   t_arg;

typedef struct s_redir
{
    t_token_type type;
    char *file;
    struct s_redir *next;
}   t_redir;

typedef struct s_cmd
{
    t_arg *arg;
    t_redir *redir;
    struct s_cmd *next;
}   t_cmd;

/*------ lexer utils ------*/
int is_space(char c);
int is_redir(char c);
t_token	*token_last(t_token *head);
void	token_add_back(t_token **head, t_token *new);
t_quote_type get_quote_type(char c);

/*------ lexer ------*/
void lexer(char *command_line, t_token **token, int *last_exit);
int add_word(t_token **head, char *str);
int add_redir(t_token **token, char chr, char nextchr);
int add_token(t_token **head, char *value, t_token_type type, t_quote_type quote);

/*------ free utils------*/
void del(char *value);
void	token_clear(t_token **token);

/*------ utils------*/
void token_error_handling(t_token **token ,int *last_exit);
void exit_program(t_token **token, t_cmd **cmd, int status);




/*------ parser------*/
int is_redirection(t_token_type type);
t_cmd	*cmd_last(t_cmd *head);
void	cmd_add_back(t_cmd **head, t_cmd *new);
t_redir	*redir_last(t_redir *head);
void	redir_add_back(t_redir **head, t_redir *new);
t_arg *arg_new(t_cmd **cmds, t_token **tokens, char *value);
t_redir *redir_new(t_cmd **cmds, t_token **tokens, t_token **current_token);
t_arg	*arg_last(t_arg *head);
void arg_add_back(t_arg **head, t_arg *new);
t_cmd *cmd_new(t_cmd **cmds, t_token **tokens);
void parser(t_cmd **cmds, t_token **tokens, int *last_exit);
int	handle_pipe(t_cmd **cmds, t_token **tokens, t_cmd **current_cmd, t_token *current_token);
int handle_redir(t_cmd **cmds, t_token **tokens,t_cmd **current_cmd, t_token **current_token);
void parser_error_handling(t_cmd **cmds,t_token **tokens, t_token *current_token, int *last_exit);
void	cmd_clear(t_cmd **cmds);
void free_cmd_redir(t_cmd **cmds);
void free_cmd_arg(t_cmd **cmds);

#endif

