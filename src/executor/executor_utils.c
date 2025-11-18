/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 18:45:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/18 19:14:15 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	ex_free_split(char **path)
{
	int	i;

	if (!path)
		return ;
	i = 0;
	while (path[i])
	{
		free(path[i]);
		path[i] = NULL;
		i++;
	}
	free(path);
	path = NULL;
}

void	free_envp_list(char **envp_list)
{
	if (!envp_list)
		return ;
	ex_free_split(envp_list);
}

t_arg	*clean_empty_args(t_arg *arg)
{
	t_arg	*tmp;
	t_arg	*next;

	tmp = arg;
	while (tmp)
	{
		next = tmp->next;
		if (tmp->expanded && tmp->quote == NONE && tmp->value[0] == '\0')
			delete_arg_node(&arg, tmp);
		tmp = next;
	}
	return (arg);
}

void	exit_program_v2(t_data *data, int status)
{
	if (data->env)
		env_clear(&data->env);
	free_all(data);
	rl_clear_history();
	exit(status);
}

void	close_fds(t_cmd *cmd)
{
	if (cmd->infile >= 0 && cmd->infile != STDIN_FILENO)
		close(cmd->infile);
	if (cmd->outfile >= 0 && cmd->outfile != STDOUT_FILENO)
		close(cmd->outfile);
}
