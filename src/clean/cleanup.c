/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 20:57:51 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/06 16:40:57 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void	reset_data(t_data *data)
{
	data->tokens = NULL;
	data->cmds = NULL;
	data->command_line = NULL;
}

void	free_all(t_data *data)
{
	if (data)
	{
		if (data->command_line)
			free(data->command_line);
		if (data->tokens)
			token_clear(&data->tokens);
		if (data->cmds)
			cmd_clear(&data->cmds);
	}
}

void	exit_program(t_data *data, int status)
{
	if (data->env)
		env_clear(&data->env);
	free_all(data);
	rl_clear_history();
	ft_putendl_fd("exit", 1);
	exit(status);
}
