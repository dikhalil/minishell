/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 01:07:40 by yocto             #+#    #+#             */
/*   Updated: 2025/11/20 10:09:44 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	handle_dot_dir(char **cmd_args, char **envp, t_data *data)
{
	int	len;

	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_args[0], 2);
	len = ft_strlen(cmd_args[0]);
	if (!ft_strcmp(cmd_args[0], "."))
	{
		ft_putendl_fd(": filename argument required", 2);
		ft_putendl_fd(".: usage: . filename [arguments]", 2);
		exit_with_error(cmd_args, envp, data, 2);
	}
	else if (!ft_strcmp(cmd_args[0], ".."))
	{
		ft_putendl_fd(": command not found", 2);
		exit_with_error(cmd_args, envp, data, 127);
	}
	else if ((len >= 2 && ((cmd_args[0][1] == '/' && cmd_args[0][0] == '.')
		|| cmd_args[0][len - 1] == '/')) || (len == 1
		&& cmd_args[0][0] == '/'))
	{
		ft_putendl_fd(": Is a directory", 2);
		exit_with_error(cmd_args, envp, data, 126);
	}
}

void	check_is_directory(char **cmd_args, char **envp, t_data *data)
{
	struct stat	st;

	if (cmd_args[0] && stat(cmd_args[0], &st) == 0)
	{
		if (S_ISDIR(st.st_mode))
			handle_dot_dir(cmd_args, envp, data);
	}
}

void	handle_trailing_slash(char **cmd_args, char **envp, t_data *data)
{
	int	len;

	if (!cmd_args[0])
		return ;
	len = ft_strlen(cmd_args[0]);
	if (len == 0 || cmd_args[0][len - 1] != '/')
		return ;
	ft_putstr_fd(cmd_args[0], 2);
	cmd_args[0][len - 1] = 0;
	if (!access(cmd_args[0], F_OK))
	{
		ft_putendl_fd(": Not a directory", 2);
		exit_with_error(cmd_args, envp, data, 126);
	}
	ft_putendl_fd(": No such file or directory", 2);
	exit_with_error(cmd_args, envp, data, 127);
}

int	check_path_access(char **cmd_args, char **envp, t_data *data)
{
	if (!is_path_format(cmd_args[0]))
		return (0);
	if (access(cmd_args[0], F_OK) != 0)
	{
		perror(cmd_args[0]);
		exit_with_error(cmd_args, envp, data, 127);
	}
	if (access(cmd_args[0], X_OK) != 0)
	{
		perror(cmd_args[0]);
		exit_with_error(cmd_args, envp, data, 126);
	}
	return (1);
}

int	check_cmd(char **cmd_args, t_data *data, char **envp)
{
	if (!cmd_args || !cmd_args[0])
	{
		ft_putendl_fd("invalid command", 2);
		exit_program_v2(data, 127);
	}
	check_is_directory(cmd_args, envp, data);
	handle_trailing_slash(cmd_args, envp, data);
	if (check_path_access(cmd_args, envp, data))
		return (1);
	if (is_executable(cmd_args[0]))
		return (1);
	return (0);
}
