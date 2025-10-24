/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 19:38:23 by yocto             #+#    #+#             */
/*   Updated: 2025/10/24 18:22:18 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	handle_input_redir(t_cmd *cmd, t_redir *redir)
{
	if (cmd->infile > STDIN_FILENO)
		close(cmd->infile);
	if (redir->type == T_IN_REDIR)
	{
		cmd->infile = open(redir->file, O_RDONLY);
		if (cmd->infile < 0)
		{
			perror(redir->file);
			return (1);
		}
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
	if (cmd->infile > STDIN_FILENO)
		close(cmd->infile);
	if (cmd->outfile > STDOUT_FILENO)
		close(cmd->outfile);
}	
int	assign_fds(t_cmd *cmd, t_cmd *has_next_cmd)
{
	int	fd[2];

	// printf("in file(%s): %d\n", cmd->arg->value, cmd->infile);
	// printf("out file(%s): %d\n", cmd->arg->value, cmd->outfile);
	if (cmd->infile == -1)
		cmd->infile = STDIN_FILENO;
	// close (cmd->outfile);
	cmd->outfile = STDOUT_FILENO;
	if (process_redirections(cmd) != 0){
		close_fds(cmd);
		return (1);
	}
	
	if (has_next_cmd && cmd->outfile == STDOUT_FILENO)
	{
		if (pipe(fd) == -1)
		{
			perror("pipe");
			return (1);
		}
		cmd->outfile = fd[1];
		cmd->next->infile = fd[0];
	}
	return (0);
}

int	check_cmd(char **cmd_args)
{
	if (!cmd_args || !cmd_args[0])
	{
		write(2, "pipex: invalid command\n", 23);
		exit(127);
	}
	if (cmd_args[0][0] == '/' || ft_strncmp(cmd_args[0], "./", 2) == 0
		|| ft_strncmp(cmd_args[0], "../", 3) == 0)
	{
		if (access(cmd_args[0], F_OK) != 0)
		{
			perror(cmd_args[0]);
			exit(127);
		}
		if (access(cmd_args[0], X_OK) != 0)
		{
			perror(cmd_args[0]);
			exit(126);
		}
		return (1);
	}
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

	path = env->value;
	paths = ft_split(path, ':');
	if (!paths)
		return (NULL);
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		path = ft_strjoin(temp, cmd);
		free(temp);
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
			ex_free_split(cmd_args);
			perror("malloc failed");
			exit(1);
		}
		arg = arg->next;
		i++;
	}
	if (check_cmd(cmd_args) == 0)
	{
		path = get_path(cmd_args[0], data->env);
		if (!path)
		{
			ex_free_split(cmd_args);
			perror("command not found");
			exit(127);
		}
	}
	else
		path = cmd_args[0];
	execve(path, cmd_args, envp);
	//if there is an error while executiion
	perror(cmd_args[0]);
	ex_free_split(cmd_args);
	free(path);
	exit(126);
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
	if (pid == 0)
	{
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
		exit(EXIT_FAILURE);
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


int executor(t_data *data)
{
	t_cmd	*command;
	int		status;
	int		final_status;
	pid_t	last_pid;
	int   i;

	last_pid = 0;
	command = data->cmds;
	final_status = 0;
	i = 0;
	while (command)
	{
		assign_fds(command, command->next);
		last_pid = fork_and_execute(command, command->next, data->envp, data);
	// if (command->next)
    // {
    //     if (command->outfile != STDOUT_FILENO)
    //         close(command->outfile);
    //     if (command->next->infile != STDIN_FILENO)
    //         close(command->next->infile);
    // }
		i++;
		command = command->next;
	}
	while (i-- > 0)
	{
		if (wait(&status) == last_pid)
			final_status = status;
	}
	return ((final_status >> 8) & 0xFF);
}
