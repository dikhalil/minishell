/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 22:01:29 by yocto             #+#    #+#             */
/*   Updated: 2025/11/18 18:55:11 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	handle_signal_status(int sig, t_data *data)
{
	if (sig == SIGINT)
	{
		data->last_exit = 130;
		write(1, "\n", 1);
	}
	else if (sig == SIGQUIT)
	{
		data->last_exit = 131;
		ft_putendl_fd("^\\Quit (core dumped)", 1);
	}
	else
		data->last_exit = 128 + sig;
}

void	process_child_status(int status, t_data *data, int last_error)
{
	int	sig;

	if (!last_error && WIFEXITED(status))
		data->last_exit = WEXITSTATUS(status);
	else if (!last_error && WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		handle_signal_status(sig, data);
	}
}

void	wait_children(pid_t last_pid, t_data *data, int last_error)
{
	pid_t	pid;
	int		status;

	pid = 1;
	while (pid > 0)
	{
		pid = waitpid(-1, &status, 0);
		if (pid == last_pid && last_pid != 0)
			process_child_status(status, data, last_error);
	}
}

int	init_and_execute(t_data *data, char ***envp, pid_t *last)
{
	t_cmd	*cmd;
	int		last_error;
	int		num_cmd;

	num_cmd = 0;
	last_error = 0;
	*last = 0;
	cmd = data->cmds;
	while (cmd)
	{
		num_cmd++;
		if (assign_fds(cmd, cmd->next) != 0)
		{
			last_error = handle_fd_error(cmd, data);
			cmd = cmd->next;
			continue ;
		}
		*last = execute_single_cmd(cmd, data, *envp, num_cmd);
		if (*last < 0)
			return (-1);
		cmd = cmd->next;
	}
	return (last_error);
}

void	executor(t_data *data)
{
	char	**envp_list;
	pid_t	last_pid;
	int		last_error;

	set_exec_signal();
	envp_list = envp_to_list(data->env);
	if (!envp_list)
		return ;
	last_error = init_and_execute(data, &envp_list, &last_pid);
	if (last_pid < 0)
	{
		ex_free_split(envp_list);
		return ;
	}
	wait_children(last_pid, data, last_error);
	free_envp_list(envp_list);
}