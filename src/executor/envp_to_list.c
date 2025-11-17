/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp_to_list.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 19:49:29 by yocto             #+#    #+#             */
/*   Updated: 2025/11/17 19:50:23 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	fill_envp(char **envp, t_env *env)
{
	int		i;
	char	*temp;
	t_env	*tmp;

	i = 0;
	tmp = env;
	while (tmp)
	{
		envp[i] = ft_strjoin(tmp->key, "=");
		if (!envp[i])
			return (0);
		temp = envp[i];
		envp[i] = ft_strjoin(envp[i], tmp->value);
		free(temp);
		if (!envp[i])
			return (0);
		i++;
		tmp = tmp->next;
	}
	return (1);
}

char	**alloc_envp(int count)
{
	char	**envp;

	envp = malloc((count + 1) * sizeof(char *));
	if (!envp)
		return (NULL);
	envp[count] = NULL;
	return (envp);
}

int	count_env(t_env *env)
{
	int		count;
	t_env	*tmp;

	count = 0;
	tmp = env;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	return (count);
}

char	**envp_to_list(t_env *env)
{
	int		count;
	char	**envp;

	count = count_env(env);
	envp = alloc_envp(count);
	if (!envp)
		return (NULL);
	if (!fill_envp(envp, env))
		return (ex_free_split(envp), NULL);
	return (envp);
}
