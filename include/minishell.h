/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 14:45:03 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/16 22:28:02 by dikhalil         ###   ########.fr       */
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
    char *value;
    t_quote_type quote;
    struct s_arg *next;
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
}   t_env;

typedef struct s_data
{
    char *command_line;
    t_env   *env;
    t_token *tokens;
    t_cmd   *cmds;
    char **argv;
    int argc;
    int     last_exit;
}   t_data;

/* ------ lexer char utils ------ */
int is_space(char c);
int is_redir(char c);

/* ------ lexer word utils ------ */
char *get_word(t_data *data, char *str, int *i, t_quote_type *quote_type);

/* ------ lexer token utils ------ */
t_token *token_last(t_token *head);
void token_add_back(t_token **head, t_token *new);
void token_clear(t_token **token);
void token_error_handling(t_data *data);

/* ------ lexer ------ */
void lexer(t_data *data);

/* ------ parser cmd utils ------ */
t_cmd *cmd_new(t_data *data);
t_cmd *cmd_last(t_cmd *head);
void cmd_add_back(t_cmd **head, t_cmd *new);
int handle_pipe(t_data *data, t_cmd **current_cmd, t_token *current_token);

/* ------ parser arg utils ------ */
t_arg *arg_new(t_data *data, t_token *current_token);
t_arg *arg_last(t_arg *head);
void arg_add_back(t_arg **head, t_arg *new);
void handle_word(t_data *data, t_cmd **current_cmd, t_token *current_token);

/* ------ parser redir utils ------ */
int is_redirection(t_token_type type);
t_redir *redir_new(t_data *data, t_token **current_token);
t_redir *redir_last(t_redir *head);
void redir_add_back(t_redir **head, t_redir *new);
int handle_redir(t_data *data, t_cmd **current_cmd, t_token **current_token);

/* ------ parser utils ------ */
void cmd_clear(t_cmd **cmds);
void parser_error_handling(t_data *data, t_token *current_token);

/* ------ parser ------ */
void parser(t_data *data);

/* ------ expand utils ------ */
char *extract_key(t_data *data, char *str, int *i);
char *extract_value(t_data *data, char *key);

/* ------ expand ------ */
void expand(t_data *data);

/* ------ env ------ */
void init_env(t_data *data, char **envp);
void env_clear(t_env **env);

/* ------ str utils ------ */
int	ft_strcmp(const char *s1, const char *s2);
char *str_join_chr(char *s, char c);
char	*str_join_free(char *s1, char *s2);
void	free_split(char **arr);
int	is_number(char *str);

/* ------ exit ------ */
void exit_program(t_data *data, int status);

#endif
