/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 15:26:48 by yocto             #+#    #+#             */
/*   Updated: 2025/11/20 10:47:02 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_pwd_deleted(t_data *data, char *oldpwd, char *target)
{
	char	*pwd_env;
	char	*new_pwd;

	pwd_env = get_env_value(data->env, "PWD");
	if (pwd_env)
	{
		new_pwd = resolve_logical_path(pwd_env, target);
		if (new_pwd)
		{
			ft_putstr_fd("cd: error retrieving current directory: ",
				STDERR_FILENO);
			ft_putendl_fd("getcwd: cannot access parent directories: "
				"No such file or directory", STDERR_FILENO);
			set_env_value(&data->env, "PWD", new_pwd);
			free(new_pwd);
		}
	}
	if (oldpwd)
	{
		set_env_value(&data->env, "OLDPWD", oldpwd);
		free(oldpwd);
	}
}

static void	update_pwd(t_data *data, char *oldpwd, char *target)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_value(&data->env, "PWD", cwd);
	else
		update_pwd_deleted(data, oldpwd, target);
	if (oldpwd && getcwd(cwd, sizeof(cwd)) != NULL)
	{
		set_env_value(&data->env, "OLDPWD", oldpwd);
		free(oldpwd);
	}
}

static int	handle_chdir_error(char *target_dir, char *oldpwd)
{
	ft_putstr_fd("cd: ", STDERR_FILENO);
	perror(target_dir);
	if (oldpwd)
		free(oldpwd);
	return (1);
}

static int	handle_target_error(char *oldpwd)
{
	if (oldpwd)
		free(oldpwd);
	return (0);
}

void	cd_builtin(t_data *data, t_arg *args)
{
	char	*target_dir;
	char	*oldpwd;

	if (validate_cd_args(args))
	{
		data->last_exit = 1;
		return ;
	}
	oldpwd = get_old_pwd(data);
	target_dir = get_target_dir(data, args);
	if (!target_dir)
	{
		handle_target_error(oldpwd);
		return ;
	}
	if (chdir(target_dir) != 0)
	{
		data->last_exit = handle_chdir_error(target_dir, oldpwd);
		return ;
	}
	update_pwd(data, oldpwd, target_dir);
	data->last_exit = 0;
}
