/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 19:38:23 by yocto             #+#    #+#             */
/*   Updated: 2025/11/20 10:19:44 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

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
