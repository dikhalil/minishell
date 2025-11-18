/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 07:27:23 by yocto             #+#    #+#             */
/*   Updated: 2025/11/18 19:21:26 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_pwd_from_env(t_data *data)
{
	t_env	*current;

	current = data->env;
	while (current)
	{
		if (ft_strcmp(current->key, "PWD") == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

void	pwd_builtin(t_data *data)
{
	char	cwd[1024];
	char	*pwd_env;

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		ft_putendl_fd(cwd, STDOUT_FILENO);
		data->last_exit = 0;
	}
	else
	{
		pwd_env = get_pwd_from_env(data);
		if (pwd_env)
		{
			ft_putendl_fd(pwd_env, STDOUT_FILENO);
			data->last_exit = 0;
		}
		else
		{
			perror("minishell: pwd");
			data->last_exit = 1;
		}
	}
}
