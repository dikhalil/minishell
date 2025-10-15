/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:11:01 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/15 22:58:04 by dikhalil         ###   ########.fr       */
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
    t_quote_type quote;
    struct s_arg    *next;
}   t_arg;

typedef struct s_redir
{
    t_token_type type;
    t_quote_type quote;
    char *file;
    struct s_redir *next;
}   t_redir;

typedef struct s_cmd
{
    t_arg *arg;
    t_redir *redir;
    struct s_cmd *next;
}   t_cmd;

 typedef struct s_env
 {
    char *key;
    char *value;
    struct s_env *next;
 }  t_env;

 typedef struct s_data
{
    t_token *tokens;
    t_cmd   *cmds;
    t_env   *env;
    int     last_exit;
}   t_data;

/*------ lexer char utils------*/
int is_space(char c);
int is_redir(char c);

/*------ lexer word utils ------*/
char	*get_word(t_token **tokens, char *str, int *i, t_quote_type *quote_type);

/*------ lexer token utils ------*/
t_token	*token_last(t_token *head);
void	token_add_back(t_token **head, t_token *new);
void	token_clear(t_token **token);
void token_error_handling(t_token **token ,int *last_exit);

/*------ lexer ------*/
void lexer(char *command_line, t_token **token, int *last_exit);

/*------ parser cmd utils------*/
t_cmd *cmd_new(t_cmd **cmds, t_token **tokens);
t_cmd	*cmd_last(t_cmd *head);
void	cmd_add_back(t_cmd **head, t_cmd *new);
int	handle_pipe(t_cmd **cmds, t_token **tokens, t_cmd **current_cmd, t_token *current_token);

/*------ parser arg utils------*/
t_arg *arg_new(t_cmd **cmds, t_token **tokens, t_token *current_token);
t_arg	*arg_last(t_arg *head);
void arg_add_back(t_arg **head, t_arg *new);
void handle_word(t_cmd **cmds, t_token **tokens, t_cmd **current_cmd, t_token *current_token);

/*------ parser redir utils------*/
int is_redirection(t_token_type type);
t_redir *redir_new(t_cmd **cmds, t_token **tokens, t_token **current_token);
t_redir	*redir_last(t_redir *head);
void	redir_add_back(t_redir **head, t_redir *new);
int handle_redir(t_cmd **cmds, t_token **tokens,t_cmd **current_cmd, t_token **current_token);

/*------ parser free utils------*/
void	cmd_clear(t_cmd **cmds);
void parser_error_handling(t_cmd **cmds,t_token **tokens, t_token *current_token, int *last_exit);

/*------ parser------*/
void parser(t_cmd **cmds, t_token **tokens, int *last_exit);

/*------ env------*/
t_env *init_env(char **envp);
void	env_clear(t_env **env);

/*------ exit------*/
void exit_program(t_token **token, t_cmd **cmd, int status);

#endif

