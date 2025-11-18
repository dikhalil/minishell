/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 18:45:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/18 18:55:11 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_single_command(int num_cmd, t_cmd *cmd)
{
	if (!cmd->next && num_cmd == 1)
		return (1);
	return (0);
}

int	handle_builtin_direct(t_cmd *cmd, t_data *data, char **envp, int num_cmd)
{
	if (is_single_command(num_cmd, cmd) && cmd->outfile == STDOUT_FILENO)
	{
		if (check_builtin(cmd, data, 0, envp))
		{
			close_fds(cmd);
			return (1);
		}
	}
	return (0);
}

int	handle_fd_error(t_cmd *cmd, t_data *data)
{
	data->last_exit = 1;
	if (!cmd->next)
		return (1);
	return (0);
}

pid_t	execute_single_cmd(t_cmd *cmd, t_data *data, char **envp, int num)
{
	pid_t	result;

	if (!cmd->arg)
		return (0);
	if (handle_builtin_direct(cmd, data, envp, num))
		return (0);
	result = fork_and_execute(cmd, cmd->next, envp, data);
	return (result);
}
