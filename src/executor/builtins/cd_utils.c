/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 18:30:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/18 18:32:22 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


char	*get_env_value(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

void	set_env_value(t_env **env, const char *key, const char *value)
{
	t_env	*tmp;
	t_env	*new;

	tmp = *env;
	while (tmp)
	{
		if (ft_strcmp(tmp->key, key) == 0)
		{
			free(tmp->value);
			tmp->value = ft_strdup(value);
			return ;
		}
		tmp = tmp->next;
	}
	new = malloc(sizeof(t_env));
	new->key = ft_strdup(key);
	new->value = ft_strdup(value);
	new->next = *env;
	*env = new;
}

char	*resolve_logical_path(char *pwd, char *target)
{
	char	*result;
	char	*last_slash;

	if (ft_strcmp(target, "..") == 0)
	{
		result = ft_strdup(pwd);
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

	if (getcwd(oldpwd, sizeof(oldpwd)) != NULL)
		return (ft_strdup(oldpwd));
	pwd_env = get_env_value(data->env, "PWD");
	if (pwd_env)
		return (ft_strdup(pwd_env));
	return (NULL);
}
