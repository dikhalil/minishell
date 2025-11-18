/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_program.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 19:30:50 by yocto             #+#    #+#             */
/*   Updated: 2025/11/18 19:06:38 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*get_command_path(char **cmd_args, t_data *data, char **envp)
{
	char	*path;

	if (check_cmd(cmd_args, data, envp) == 0)
	{
		path = get_path(cmd_args[0], data->env);
		if (!path)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd_args[0], 2);
			ft_putendl_fd(": command not found", 2);
			ex_free_split(cmd_args);
			free_envp_list(envp);
			exit_program_v2(data, 127);
		}
		return (path);
	}
	return (cmd_args[0]);
}

void	handle_execve_failure(char **cmd_args, char *path, char **envp,
		t_data *data)
{
	perror(cmd_args[0]);
	if (path != cmd_args[0])
		free(path);
	ex_free_split(cmd_args);
	free_envp_list(envp);
	exit_program_v2(data, 126);
}

int	execute_program(t_arg *arg, char **envp, t_data *data)
{
	char	**cmd_args;
	char	*path;

	arg = clean_empty_args(arg);
	if (!arg)
	{
		free_envp_list(envp);
		exit_program_v2(data, 0);
	}
	cmd_args = build_cmd_args(arg);
	if (!cmd_args)
		return (1);
	path = get_command_path(cmd_args, data, envp);
	execve(path, cmd_args, envp);
	handle_execve_failure(cmd_args, path, envp, data);
	return (0);
}
