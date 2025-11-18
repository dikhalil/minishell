/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 19:38:23 by yocto             #+#    #+#             */
/*   Updated: 2025/11/18 19:10:57 by dikhalil         ###   ########.fr       */
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
		if (ft_strchr(redir->file, ' ') != NULL && redir->quote == NONE)
		{
			write(2, "minishell: ", 11);
			write(2, redir->file, ft_strlen(redir->file));
			write(2, ": ambiguous redirect\n", 21);
			return (1);
		}
		cmd->infile = open(redir->file, O_RDONLY);
		if (cmd->infile < 0)
		{
			write(2, "minishell: ", 11);
			write(2, redir->file, ft_strlen(redir->file));
			write(2, ": No such file or directory\n", 29);
			return (1);
		}
	}
	return (0);
}

static int	handle_output_redir(t_cmd *cmd, t_redir *redir)
{
	if (cmd->outfile > STDOUT_FILENO)
		close(cmd->outfile);
	if (ft_strchr(redir->file, ' ') != NULL && redir->quote == NONE)
	{
		write(2, "minishell: ", 11);
		write(2, redir->file, ft_strlen(redir->file));
		write(2, ": ambiguous redirect\n", 21);
		return (1);
	}
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

int	assign_fds(t_cmd *cmd, t_cmd *has_next_cmd)
{
	int	fd[2];

	if (cmd->infile == -1)
		cmd->infile = STDIN_FILENO;
	cmd->outfile = STDOUT_FILENO;
	if (has_next_cmd)
	{
		if (pipe(fd) == -1)
		{
			perror("pipe");
			return (1);
		}
		if (cmd->outfile == STDOUT_FILENO)
			cmd->outfile = fd[1];
		else
			close(fd[1]);
		cmd->next->infile = fd[0];
	}
	if (process_redirections(cmd) != 0)
	{
		close_fds(cmd);
		return (1);
	}
	return (0);
}

int	check_builtin(t_cmd *command, t_data *data, int ischild, char **envp)
{
	if (!command || !command->arg || !command->arg->value)
		return (0);
	if (ft_strcmp(command->arg->value, "cd") == 0)
		cd_builtin(data, command->arg->next);
	else if (ft_strcmp(command->arg->value, "echo") == 0)
		echo_builtin(data, command->arg->next);
	else if (ft_strcmp(command->arg->value, "env") == 0)
		env_builtin(data, data->env, command->arg);
	else if (ft_strcmp(command->arg->value, "exit") == 0)
		exit_builtin(data, command->arg->next, ischild, envp);
	else if (ft_strcmp(command->arg->value, "unset") == 0)
		unset_builtin(data, command->arg->next);
	else if (ft_strcmp(command->arg->value, "export") == 0)
		export_builtin(data, command->arg->next);
	else if (ft_strcmp(command->arg->value, "pwd") == 0)
		pwd_builtin(data);
	else
		return (0);
	return (1);
}
