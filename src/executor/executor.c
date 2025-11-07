/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 13:57:48 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/07 14:43:44 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include <sys/stat.h>

static int	handle_input_redir(t_cmd *cmd, t_redir *redir)
{
	if (cmd->infile > STDIN_FILENO)
		close(cmd->infile);
	if (redir->type == T_IN_REDIR)
	{
		cmd->infile = open(redir->file, O_RDONLY);
		if (cmd->infile < 0){
			write(2, "minishell: ", 11);
			write(2, redir->file, ft_strlen(redir->file));
			write(2, ": No such file or directory\n", 29);
			return (1);
		}
		//i will go with this type of errors on the next errors like the real bash
	}	
	return (0);
}

static int	handle_output_redir(t_cmd *cmd, t_redir *redir)
{
	if (cmd->outfile > STDOUT_FILENO)
		close(cmd->outfile);
	if (redir->type == T_OUT_REDIR)
		cmd->outfile = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == T_APPEND)
		cmd->outfile = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (cmd->outfile < 0)
	{
		perror(redir->file);
		return (1);
	}
	return (0);
}

static int	process_redirections(t_cmd *cmd)
{
	t_redir	*redir;

	redir = cmd->redir;
	while (redir)
	{
		if (redir->type == T_IN_REDIR)
		{
			if (handle_input_redir(cmd, redir) != 0)
				return (1);
		}
		else if (redir->type == T_OUT_REDIR || redir->type == T_APPEND)
		{
			if (handle_output_redir(cmd, redir) != 0)
				return (1);
		}
		redir = redir->next;
	}
	return (0);
}

void close_fds(t_cmd *cmd)
{
    if (cmd->infile >= 0 && cmd->infile != STDIN_FILENO)
        close(cmd->infile);
    if (cmd->outfile >= 0 && cmd->outfile != STDOUT_FILENO)
        close(cmd->outfile);
}

int	assign_fds(t_cmd *cmd, t_cmd *has_next_cmd)
{
	int	fd[2];

	if (cmd->infile == -1)
		cmd->infile = STDIN_FILENO;
	// close (cmd->outfile);
	cmd->outfile = STDOUT_FILENO;
	if (has_next_cmd)
	{
		if (pipe(fd) == -1)
		{
			perror("pipe");
			return (1);
		}
		if(cmd->outfile == STDOUT_FILENO)
			cmd->outfile = fd[1];
		else
			close(fd[1]);
		cmd->next->infile = fd[0];
	}
	if (process_redirections(cmd) != 0){
		close_fds(cmd);
		return (1);
	}
	return (0);
}

int	check_cmd(char **cmd_args, t_data *data, char **envp)
{
	struct stat st;
	
	if (!cmd_args || !cmd_args[0])
	{
		ft_putstr_fd(cmd_args[0], 2);
		ft_putendl_fd("invalid command", 2);
		exit_program_v2(data, 127);
	}
	
	if (stat(cmd_args[0], &st) == 0)
	{
    	if (S_ISDIR(st.st_mode))
    	{
        	ft_putstr_fd(cmd_args[0], 2);
        	ft_putendl_fd(": Is a directory\n", 2);
        	ex_free_split(cmd_args);
			ex_free_split(envp);
        	exit_program_v2(data, 126);
    	}
	}
	if (  cmd_args[0][0] == '/' || ft_strncmp(cmd_args[0], "./", 2) == 0
		|| ft_strncmp(cmd_args[0], "../", 3) == 0)
	{
		if (access(cmd_args[0], F_OK) != 0)
		{
			perror(cmd_args[0]);
			ex_free_split(cmd_args);
			ex_free_split(envp);
			exit_program_v2(data, 127);
		}
		if (access(cmd_args[0], X_OK) != 0)
		{
			perror(cmd_args[0]);
			ex_free_split(cmd_args);
			ex_free_split(envp);
			exit_program_v2(data, 126);
		}
		return (1);
	}
	if(cmd_args[0][0])
		if (access(cmd_args[0], F_OK) == 0 && access(cmd_args[0], X_OK) == 0)
			return 1;
	return (0);
}

void ex_free_split(char **path)
{
	int i;

	i = 0;
	while(path[i]){
		free(path[i]);
		i++;	
	}
	free(path);
}
void free_envp_list(char **envp_list)
{
    if (!envp_list)
        return;
    ex_free_split(envp_list);
}
char	*get_path(char *cmd, t_env *env)
{
	char *path;
	char **paths;
	int i;
	char *temp;

	if (!cmd || !*cmd)
		return (NULL);
	i = 0;
	while (env && ft_strcmp(env->key, "PATH") != 0)
   		env = env->next;
	if (!env || !env->value)
        return (NULL);
	path = env->value;
	paths = ft_split(path, ':');
	if (!paths)
		return (NULL);
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		if (!temp)
        {
            ex_free_split(paths);
            return (NULL);
        }
		path = ft_strjoin(temp, cmd);
		free(temp);
		if (!path)
        {
            ex_free_split(paths);
            return (NULL);
        }
		if (access(path, F_OK) == 0)
		{
			ex_free_split(paths);
			return (path);
		}
		free(path);
		i++;
	}
	ex_free_split(paths);
	return (NULL);
}

int execute_program(t_arg *arg, char **envp, t_data *data)
{
	char	**cmd_args;
	int		i;
	int		count;
	char	*path;
	t_arg	*tmp;

	tmp = arg;
	i = 0;
	while (tmp)
	{
		i++;
		tmp = tmp->next;
	}
	count = i;
	cmd_args = malloc((count + 1) * sizeof(char *));
	if (!cmd_args)
	{
		perror("malloc failed");
		return (1);
	}
	cmd_args[count] = NULL;
	i = 0;
	while (i < count)
	{
		cmd_args[i] = ft_strdup(arg->value);
		if (!cmd_args[i])
		{
			cmd_args[i] = NULL;
			ex_free_split(cmd_args);
			perror("malloc failed");
			exit(1);
		}
		arg = arg->next;
		i++;
	}
	if (check_cmd(cmd_args, data, envp) == 0)
	{
		path = get_path(cmd_args[0], data->env);
		if (!path)
		{
			ft_putstr_fd(cmd_args[0],2);
			if (ft_strchr(cmd_args[0] , '/'))
				ft_putendl_fd(" :No such file or directory",2);
			else		
				ft_putendl_fd(" :command not found", 2);
			ex_free_split(cmd_args);
			free_envp_list(envp);
			exit_program_v2(data, 127);
		}
	}
	else
		path = cmd_args[0];
	execve(path, cmd_args, envp);
	//if there is an error while executiion
	perror(cmd_args[0]);
	ex_free_split(cmd_args);
	free(path);
	exit_program_v2(data, 126);
	return (0);
}

int	fork_and_execute(t_cmd *command, t_cmd *next, char **envp, t_data *data)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	set_exec_signal();
	if(pid == 0 && isBuiltin(command))
	{
		set_child_signal();
		if (next)
			close(next->infile);
		if (command->infile != STDIN_FILENO)
		{
			dup2(command->infile, STDIN_FILENO);
			close(command->infile);
		}
		
		if (command->outfile != STDOUT_FILENO)
		{
			dup2(command->outfile, STDOUT_FILENO);
			close(command->outfile);
		}
		check_builtin_pipe(command, data);
		close_fds(command);
		exit_program_v2(data, EXIT_SUCCESS);
	}
	else
	if (pid == 0)
	{
		set_child_signal();
		if (next)
			close(next->infile);
		if (command->infile != STDIN_FILENO)
		{
			dup2(command->infile, STDIN_FILENO);
			close(command->infile);
		}
		if (command->outfile != STDOUT_FILENO)
		{
			dup2(command->outfile, STDOUT_FILENO);
			close(command->outfile);
		}
		execute_program(command->arg, envp, data);
		close_fds(command);
		exit_program_v2(data, EXIT_FAILURE);
	}
	else
	{
		if (command->outfile != STDOUT_FILENO)
			close(command->outfile);
		if (command->infile != STDIN_FILENO)
			close(command->infile);
	}
	return (pid);
}
char **envp_to_list(t_env *env)
{
	int		count;
	char	**envp;
	t_env	*tmp;
	int		i;

	count = 0;
	tmp = env;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	envp = malloc((count + 1) * sizeof(char *));
	if (!envp)
		return (NULL);
	envp[count] = NULL;
	i = 0;
	tmp = env;
	while (tmp)
	{
		envp[i] = ft_strjoin(tmp->key, "=");
		if (!envp[i])
        {
            ex_free_split(envp);
            return (NULL);
        }
		char *temp = envp[i];
		envp[i] = ft_strjoin(envp[i], tmp->value);
		free(temp);
		if (!envp[i])
        {
            ex_free_split(envp);
            return (NULL);
        }
		i++;
		tmp = tmp->next;
	}
	return (envp);
}
int isBuiltin(t_cmd *command)
{
	if (ft_strcmp(command->arg->value, "echo") == 0)
		return (1);
	else if (ft_strcmp(command->arg->value, "env") == 0)
		return (1);
	return (0);
}
int check_builtin_pipe(t_cmd *command, t_data *data)
{
	if (ft_strcmp(command->arg->value, "echo") == 0){
		echo_builtin(data, command->arg->next);
		return (1);
	}
	else if (ft_strcmp(command->arg->value, "env") == 0){
		env_builtin(data->env);
		return (1);
	}
	return (0);
}

int check_builtin(t_cmd *command, t_data *data)
{
	if(command -> next)
		return (1);
	if (ft_strcmp(command->arg->value, "cd") == 0)
	{
		cd_builtin(data, command ->arg->next);

		return (1);
	}
	else if (ft_strcmp(command->arg->value, "exit") == 0)
	{
		if (command->arg->next)
			exit_builtin(data, ft_atoi(command->arg->next->value));
		else
			exit_program(data, data->last_exit);
	}
	// else if (ft_strcmp(command->arg->value, "export") == 0)
	// {
	// 	export_builtin(data, command->arg->next);
	// 	return (1);
	// }
	else if (ft_strcmp(command->arg->value, "unset") == 0)
	{
		unset_builtin(data, command->arg->next);
		return (1);
	}
	return (0);
}
void exit_program_v2(t_data *data, int status)
{

    if (data->env)
        env_clear(&data->env);
    free_all(data);
    exit(status);
}

void executor(t_data *data)
{
	t_cmd	*command;
	int		status;
	pid_t	last_pid;
	int sig;
	char	**envp_list;
	pid_t pid;

	set_exec_signal();
	envp_list = envp_to_list(data->env);
	if (!envp_list)
		return ;
	last_pid = 0;
	command = data->cmds;
	while (command)
	{
		if(assign_fds(command, command->next) != 0)
		{
			command = command->next;
			continue;
		}
		if (command->arg)
		{
			if (check_builtin(command, data))
			{
				close_fds(command);
				last_pid = 0;
				command = command->next;
				continue;
			}
			if (!command->next && check_builtin_pipe(command, data))
			{
				close_fds(command);
				last_pid = 0;
				command = command->next;
				continue;
			}
			last_pid = fork_and_execute(command, command->next, envp_list, data);
			if (last_pid < 0)
			{
				ex_free_split(envp_list);
				return ;
			}
		}
		else
			last_pid = 0;
		command = command->next;
	}
	pid = 1;
	while (pid > 0)
	{
		pid = waitpid(-1, &status, 0);
		if (pid == last_pid && last_pid != 0)
		{
			if (WIFSIGNALED(status)) 
			{
				sig = WTERMSIG(status);
				if (sig == SIGINT)
				{
					data->last_exit = 130;
					write(1, "\n", 1); 
				}
				else if (sig == SIGQUIT)
				{
					data->last_exit = 131;
					write(1, "Quit (core dumped)\n", 19);
				}
				else
					data->last_exit = 128 + sig;
			}
			else if (WIFEXITED(status))
				data->last_exit = WEXITSTATUS(status);
		}
		else if (last_pid == 0)
			data->last_exit = 0;
	}
	free_envp_list(envp_list);
}

