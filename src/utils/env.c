/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 22:41:21 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/15 22:43:07 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static t_env	*env_new(char *key, char *value, t_env **env)
{
	t_env	*new;

	new = malloc(sizeof(t_env));
	if (!new)
    {
        free(key);
        exit_program(env, NULL, NULL, ERR_MEM);
    }
	new->key = ft_strdup(key);
    if (!new->key)
    {
        free(key);
        exit_program(env, NULL, NULL, ERR_MEM);
    }    
    new->value = ft_strdup(value);
    if (!new->value)
    {
        free(key);
        exit_program(env, NULL, NULL, ERR_MEM);
    }	
    new->next = NULL;
	return (new);
}

static t_env	*env_last(t_env *env)
{
	if (!env)
		return (NULL);
	while (env->next)
		env = env->next;
	return (env);
}

static void	env_add_back(t_env **env, t_env *new)
{
	t_env	*last;

	if (!env || !new)
		return ;
	if (!*env)
	{
		*env = new;
		return ;
	}
	last = env_last(*env);
	last->next = new;
}

void	env_clear(t_env **env)
{
    t_env	*tmp;
    
	if (!env)
		return ;
    while (*env)
	{
        free((*env)->key);
		free((*env)->value);
		tmp = *env;
		*env = (*env)->next;
		free(tmp);
	}
	*env = NULL;
}

t_env *init_env(char **envp)
{
    int i;
    t_env *env;
    char *equal;
    char *tmp;

    env = NULL;
    i = 0;
    while (envp[i])
    {
        tmp = ft_strdup(envp[i]);
        if (!tmp)
            exit_program(env, NULL, NULL, ERR_MEM);
        equal = ft_strchr(tmp, '=');
        if (equal)
        {
            equal = '\0';
            env_add_back(&env, env_new(tmp, equal + 1, &env));
        }
        else
            env_add_back(&env, env_new(tmp, "", &env));
        free(tmp);
        i++;
    }
    return (env);
}
