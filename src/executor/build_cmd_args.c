/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_cmd_args.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 18:45:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/18 18:55:11 by dikhalil         ###   ########.fr       */
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
