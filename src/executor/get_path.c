/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 19:07:35 by yocto             #+#    #+#             */
/*   Updated: 2025/11/17 19:11:11 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*find_path_env(t_env *env)
{
	while (env && ft_strcmp(env->key, "PATH") != 0)
		env = env->next;
	if (!env || !env->value)
		return (NULL);
	return (env->value);
}

char	*build_full_path(char *dir, char *cmd)
{
	char	*temp;
	char	*path;

	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	path = ft_strjoin(temp, cmd);
	free(temp);
	return (path);
}

char	*search_in_paths(char **paths, char *cmd)
{
	char	*path;
	int		i;

	i = 0;
	while (paths[i])
	{
		path = build_full_path(paths[i], cmd);
		if (!path)
		{
			ex_free_split(paths);
			return (NULL);
		}
		if (access(path, F_OK) == 0)
		{
			ex_free_split(paths);
			return (path);
		}
		free(path);
		i++;
	}
	return (NULL);
}

char	*get_path(char *cmd, t_env *env)
{
	char	*path_env;
	char	**paths;
	char	*result;

	if (!cmd || !*cmd)
		return (NULL);
	path_env = find_path_env(env);
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	result = search_in_paths(paths, cmd);
	if (!result)
		ex_free_split(paths);
	return (result);
}