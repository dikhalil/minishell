/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 14:45:03 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/19 21:54:49 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include <fcntl.h>
# include <stdio.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stddef.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

# define TRUE 1
# define ERR_MEM 1
# define PROMPT "minishell$ "

typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_IN_REDIR,
	T_OUT_REDIR,
	T_APPEND,
	T_HEREDOC,
}					t_token_type;

typedef enum e_quote_type
{
	NONE,
	SINGLE_QUOTE,
	DOUBLE_QUOTE
}					t_quote_type;

typedef struct s_word_state
{
	char			*word;
	t_quote_type	quote_type;
	int				expanded;
}					t_word_state;

typedef struct s_token
{
	char			*value;
	int				expanded;
	t_token_type	type;
	t_quote_type	quote;
	struct s_token	*next;
}					t_token;

typedef struct s_arg
{
	char			*value;
	t_quote_type	quote;
	int				expanded;
	struct s_arg	*next;
}					t_arg;

typedef struct s_redir
{
	t_token_type	type;
	t_quote_type	quote;
	char			*file;
	char			*delim;
	struct s_redir	*next;
}					t_redir;

typedef struct s_cmd
{
	t_arg			*arg;
	t_redir			*redir;
	int				infile;
	int				outfile;
	struct s_cmd	*next;
}					t_cmd;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}					t_env;

typedef struct s_data
{
	char			*command_line;
	t_env			*env;
	t_token			*tokens;
	t_cmd			*cmds;
	char			**argv;
	int				argc;
	int				last_exit;
}					t_data;

/* ------ global var ------ */
extern int			g_sig;

/* ------ lexer char utils ------ */
int					is_space(char c);
int					is_redir(char c);
int					is_redirection(t_token_type type);

/* ------ lexer token utils ------ */
t_token			*token_last(t_token *head);
void				token_add_back(t_token **head, t_token *new);
void				token_clear(t_token **token);
void				token_error_handling(t_data *data);
int				add_token(t_data *data, char *value, t_token_type type,
						t_quote_type quote);

/* ------ lexer word/extract utils ------ */
char				*extract_word(t_data *data, char *str, int *i,
						t_quote_type *quote_type);
int					add_word(t_data *data, char *str);

/* ------ lexer word split utils ------ */
void				process_split_array(t_data *data, char **splited,
						char **word);
int					check_split_content(char **splited);
void				extract_and_join_prefix(t_data *data, char *part,
						char **word, int *first_space);
void				handle_split_part(t_data *data, char *part, char **word);

/* ------ lexer ------ */
void				lexer(t_data *data);

/* ------ executor utils ------*/
char			*get_path(char *cmd, t_env *env);
int				execute_program(t_arg **arg, char **envp, t_data *data);
int				fork_and_execute(t_cmd *command, t_cmd *next, char **envp,
						t_data *data);
void			executor(t_data *data);
void			ex_free_split(char **path);
void			free_and_exit(t_data *data, char **splited, char **word);
void			close_fds(t_cmd *cmd);
int				assign_fds(t_cmd *cmd, t_cmd *has_next_cmd);
int				check_cmd(char **cmd_args, t_data *data, char **envp);
void			exit_program_v2(t_data *data, int status);
void			exit_with_error(char **cmd_args, char **envp, t_data *data,
						int code);
void			handle_dot_dir(char **cmd_args, char **envp, t_data *data);
void			check_is_directory(char **cmd_args, char **envp,
						t_data *data);
void			handle_trailing_slash(char **cmd_args, char **envp,
						t_data *data);
void				cmd_add_back(t_cmd **head, t_cmd *new);
int					handle_pipe(t_data *data, t_cmd **current_cmd,
						t_token *current_token);

/* ------ parser arg utils ------ */
t_cmd			*cmd_new(t_data *data);
t_cmd			*cmd_last(t_cmd *head);

t_arg				*arg_new(t_data *data, t_cmd **current_cmd,
						t_token *current_token);
t_arg				*arg_last(t_arg *head);
void				arg_add_back(t_arg **head, t_arg *new);
void				handle_word(t_data *data, t_cmd **current_cmd,
						t_token *current_token);

/* ------ parser redir utils ------ */
t_redir				*redir_new(t_data *data, t_cmd **current_cmd,
						t_token **current_token);
t_redir				*redir_last(t_redir *head);
void				redir_add_back(t_redir **head, t_redir *new);
int					handle_redir(t_data *data, t_cmd **current_cmd,
						t_token **current_token);

/* ------ parser utils ------ */
void				cmd_clear(t_cmd **cmds);
void				parser_error_handling(t_data *data, t_token *current_token);

/* ------ parser ------ */
void				parser(t_data *data);

/* ------ heredoc utils ------ */
void				handle_heredoc(t_data *data, t_cmd *cmd, t_redir *redir);

/* ------ heredoc ------ */
void				heredoc(t_data *data);

/* ------ expand extract ------ */
char				*extract_key(t_data *data, char *str, int *i);
char				*extract_value(t_data *data, char *key);

/* ------ expand str ------ */
void				expand_str(t_data *data, char **str);

/* ------ env ------ */
void				init_env(t_data *data, char **envp);
void				env_clear(t_env **env);
void				increment_shlvl(t_data *data);
t_env				*env_new(t_data *data, char *key, char *value);
void				env_add_back(t_env **env, t_env *new);

/* ------ signals ------ */
void				set_main_signal(void);
void				set_heredoc_signal(void);
void				set_child_signal(void);
void				set_exec_signal(void);

/* ------ shell ------ */
void				run_shell(t_data *data);

/* ------ executor utils ------*/
char				*get_path(char *cmd, t_env *env);
int					execute_program(t_arg **arg, char **envp, t_data *data);
int					fork_and_execute(t_cmd *command, t_cmd *next, char **envp,
						t_data *data);
void				executor(t_data *data);
void				ex_free_split(char **path);
void				free_and_exit(t_data *data, char **splited, char **word);
void				close_fds(t_cmd *cmd);
int					assign_fds(t_cmd *cmd, t_cmd *has_next_cmd);
int					check_cmd(char **cmd_args, t_data *data, char **envp);
void				exit_program_v2(t_data *data, int status);
void				exit_with_error(char **cmd_args, char **envp, t_data *data,
						int code);
void				handle_dot_dir(char **cmd_args, char **envp, t_data *data);
void				check_is_directory(char **cmd_args, char **envp,
						t_data *data);
void				handle_trailing_slash(char **cmd_args, char **envp,
						t_data *data);
int					is_path_format(char *cmd);
int					check_path_access(char **cmd_args, char **envp,
						t_data *data);
int					is_executable(char *cmd);
char				*find_path_env(t_env *env);
char				*build_full_path(char *dir, char *cmd);
char				*search_in_paths(char **paths, char *cmd);
t_arg				*find_prev_node(t_arg *head, t_arg *target, t_arg **prev);
void				delete_arg_node(t_arg **head, t_arg *target);
int					count_args(t_arg *arg);
char				**allocate_cmd_args(int count);
int					fill_cmd_args(char **cmd_args, t_arg *arg, int count);
char				**build_cmd_args(t_arg *arg);
char			*get_command_path(char **cmd_args, t_data *data,
						char **envp);
void				handle_execve_failure(char **cmd_args, char *path,
						char **envp, t_data *data);
void				free_envp_list(char **envp_list);
t_arg			*clean_empty_args(t_arg **arg);
void				handle_child_process(t_cmd *cmd, t_cmd *next, char **envp,
						t_data *data);
void				apply_redirections(t_cmd *cmd);
void				handle_parent_process(t_cmd *cmd);
int					fill_envp(char **envp, t_env *env);
char				**alloc_envp(int count);
int					count_env(t_env *env);
char				**envp_to_list(t_env *env);
int					init_and_execute(t_data *data, char ***envp, pid_t *last);
pid_t				execute_single_cmd(t_cmd *cmd, t_data *data, char **envp,
						int num);
void				wait_children(pid_t last_pid, t_data *data, int last_error);
int					handle_fd_error(t_cmd *cmd, t_data *data);
void				process_child_status(int status, t_data *data,
						int last_error);
void				handle_signal_status(int sig, t_data *data);
int					handle_builtin_direct(t_cmd *cmd, t_data *data, char **envp,
						int num_cmd);
int					is_single_command(int num_cmd, t_cmd *cmd);

/* ------ builtins ------ */
int					is_builtin(t_cmd *command);
int					check_builtin(t_cmd *command, t_data *data, int ischild,
						char **envp);
void				cd_builtin(t_data *data, t_arg *args);
void				echo_builtin(t_data *data, t_arg *args);
void				env_builtin(t_data *data, t_env *env, t_arg *args);
void				unset_builtin(t_data *data, t_arg *args);
int					ft_isnumber(const char *s);
void				exit_builtin(t_data *data, t_arg *arg, int is_child,
						char **envp);
void				pwd_builtin(t_data *data);

/* ------ builtin export ------ */
void				export_builtin(t_data *data, t_arg *args);
void				add_or_update_env(t_data *data, const char *arg);
void				print_env_sorted(t_env *env);
int					valid_identifier(const char *str);
char				*strdup_until_char(const char *str, char c);
void				swap_env_nodes(t_env *current, t_env *next);
void				sort_env_list(t_env *env);
void				print_env_entry(t_env *current);

/* ------ builtin cd/env utils ------ */
char				*get_env_value(t_env *env, const char *key);
void				set_env_value(t_env **env, const char *key,
						const char *value);
char				*resolve_logical_path(char *pwd, char *target);
char				*get_target_dir(t_data *data, t_arg *args);
char				*get_old_pwd(t_data *data);

/* ------ cleanup ------ */
void				reset_data(t_data *data);
void				free_all(t_data *data);
void				exit_program(t_data *data, int status);

#endif
