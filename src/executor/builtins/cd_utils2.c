/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 10:45:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/20 10:47:02 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*resolve_logical_path(char *pwd, char *target)
{
	char	*result;
	char	*last_slash;

	if (ft_strcmp(target, "..") == 0)
	{
		result = ft_strdup(pwd);
		if (!result)
			return (NULL);
		last_slash = ft_strrchr(result, '/');
		if (last_slash && last_slash != result)
			*last_slash = '\0';
		else if (last_slash == result)
			result[1] = '\0';
		return (result);
	}
	return (NULL);
}

char	*get_target_dir(t_data *data, t_arg *args)
{
	char	*target_dir;

	if (args == NULL || args->value == NULL)
		target_dir = get_env_value(data->env, "HOME");
	else if (ft_strcmp(args->value, "-") == 0)
	{
		target_dir = get_env_value(data->env, "OLDPWD");
		if (target_dir)
			ft_putendl_fd(target_dir, STDOUT_FILENO);
	}
	else if (ft_strcmp(args->value, "~") == 0)
		target_dir = get_env_value(data->env, "HOME");
	else
		return (args->value);
	if (!target_dir)
	{
		if (args == NULL || args->value == NULL
			|| ft_strcmp(args->value, "~") == 0)
			write(STDERR_FILENO, "cd: HOME not set\n", 17);
		else
			write(STDERR_FILENO, "cd: OLDPWD not set\n", 19);
		data->last_exit = 1;
	}
	return (target_dir);
}

char	*get_old_pwd(t_data *data)
{
	char	oldpwd[1024];
	char	*pwd_env;
	char	*result;

	if (getcwd(oldpwd, sizeof(oldpwd)) != NULL)
	{
		result = ft_strdup(oldpwd);
		return (result);
	}
	pwd_env = get_env_value(data->env, "PWD");
	if (pwd_env)
	{
		result = ft_strdup(pwd_env);
		return (result);
	}
	return (NULL);
}

int	validate_cd_args(t_arg *args)
{
	if (args && args->next)
	{
		ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	return (0);
}
