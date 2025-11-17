/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_program.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 19:30:50 by yocto             #+#    #+#             */
/*   Updated: 2025/11/17 20:51:33 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	count_args(t_arg *arg)
{
	int		count;
	t_arg	*tmp;

	count = 0;
	tmp = arg;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	return (count);
}

char	**allocate_cmd_args(int count)
{
	char	**cmd_args;

	cmd_args = malloc((count + 1) * sizeof(char *));
	if (!cmd_args)
	{
		perror("malloc failed");
		return (NULL);
	}
	cmd_args[count] = NULL;
	return (cmd_args);
}

int	fill_cmd_args(char **cmd_args, t_arg *arg, int count)
{
	t_arg	*tmp;
	int		i;

	tmp = arg;
	i = 0;
	while (i < count)
	{
		cmd_args[i] = ft_strdup(tmp->value);
		if (!cmd_args[i])
		{
			cmd_args[i] = NULL;
			ex_free_split(cmd_args);
			perror("malloc failed");
			exit(1);
		}
		tmp = tmp->next;
		i++;
	}
	return (0);
}

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

char	**build_cmd_args(t_arg *arg)
{
	char	**cmd_args;
	int		count;

	count = count_args(arg);
	cmd_args = allocate_cmd_args(count);
	if (!cmd_args)
		return (NULL);
	fill_cmd_args(cmd_args, arg, count);
	return (cmd_args);
}

void	handle_execve_failure(char **cmd_args, char *path, t_data *data)
{
	perror(cmd_args[0]);
	if (path != cmd_args[0])
		free(path);
	ex_free_split(cmd_args);
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
	handle_execve_failure(cmd_args, path, data);
	return (0);
}