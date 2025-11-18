/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork_and_execute.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 19:38:17 by yocto             #+#    #+#             */
/*   Updated: 2025/11/18 18:55:34 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	handle_parent_process(t_cmd *cmd)
{
	if (cmd->outfile != STDOUT_FILENO)
		close(cmd->outfile);
	if (cmd->infile != STDIN_FILENO)
		close(cmd->infile);
}

void	apply_redirections(t_cmd *cmd)
{
	if (cmd->infile != STDIN_FILENO)
	{
		dup2(cmd->infile, STDIN_FILENO);
		close(cmd->infile);
	}
	if (cmd->outfile != STDOUT_FILENO)
	{
		dup2(cmd->outfile, STDOUT_FILENO);
		close(cmd->outfile);
	}
}

void	handle_child_process(t_cmd *cmd, t_cmd *next, char **envp, t_data *data)
{
	set_child_signal();
	if (next)
		close(next->infile);
	apply_redirections(cmd);
	if (is_builtin(cmd))
	{
		check_builtin(cmd, data, 1, envp);
		close_fds(cmd);
		ex_free_split(envp);
		exit_program_v2(data, data->last_exit);
	}
	execute_program(cmd->arg, envp, data);
	close_fds(cmd);
	ex_free_split(envp);
	exit_program_v2(data, EXIT_SUCCESS);
}

int	fork_and_execute(t_cmd *command, t_cmd *next, char **envp, t_data *data)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (perror("fork"), -1);
	set_exec_signal();
	if (pid == 0)
		handle_child_process(command, next, envp, data);
	else
		handle_parent_process(command);
	return (pid);
}
