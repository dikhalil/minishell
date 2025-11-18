/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_cmd_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 18:45:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/18 18:55:11 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	exit_with_error(char **cmd_args, char **envp, t_data *data, int code)
{
	ex_free_split(cmd_args);
	ex_free_split(envp);
	exit_program_v2(data, code);
}

int	is_path_format(char *cmd)
{
	if (!cmd)
		return (0);
	if (cmd[0] == '/')
		return (1);
	if (ft_strncmp(cmd, "./", 2) == 0)
		return (1);
	if (ft_strncmp(cmd, "../", 3) == 0)
		return (1);
	return (0);
}

int	is_executable(char *cmd)
{
	if (!cmd || !cmd[0])
		return (0);
	if (access(cmd, F_OK) == 0 && access(cmd, X_OK) == 0)
		return (1);
	return (0);
}
