/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_extract.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 18:01:04 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/07 14:25:59 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

char *extract_key(t_data *data, char *str, int *i)
{
    char *key;
    int start;

    if (str[*i] == '?')
    {
        key = ft_strdup("?");
        (*i)++;
    }
    else if (!str[*i] || (!ft_isalnum(str[*i]) && str[*i] != '_'))
        key = ft_strdup("$");
    else if (ft_isdigit(str[*i]))
    {
        key = ft_substr(str, *i, 1); 
        (*i)++;
    }
    else
    {
        start = *i;
        while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
            (*i)++;
        key = ft_substr(str, start, *i - start);
    }
    if(!key)
        exit_program(data, ERR_MEM);
    return (key);
}

static char *extract_special(t_data *data, char *key)
{
    int num;
    char *value;

    value = NULL;
    if (!ft_strcmp(key, "$"))
        value = ft_strdup(key);
    else if (!ft_strcmp(key, "?"))
        value = ft_itoa(data->last_exit);
    else if (ft_isdigit(key[0]))
    {
        num = ft_atoi(key);
        if (num >= 0 && num < data->argc)
            value = ft_strdup(data->argv[num]);
        else
            value = ft_strdup("");
    }
    else
        return (NULL);
    if (!value)
        exit_program(data, ERR_MEM);
    return (value);
}

static char *extract_env(t_data *data, char *key)
{
    t_env *env;
    char *value;
    
    env = data->env;
    while (env)
    {
        if (ft_strcmp(env->key, key) == 0)
        {
            value = ft_strdup(env->value);
            if (!value)
                exit_program(data, ERR_MEM);
            return (value);
        }
        env = env->next;
    }
    value = ft_strdup("");
    if (!value)
        exit_program(data, ERR_MEM);
    return (value);
}

char *extract_value(t_data *data, char *key)
{
    char *value;

    value = extract_special(data, key);
    if (!value)
        value = extract_env(data, key);
    free(key);
    if (!value)
        exit_program(data, ERR_MEM);
    return (value);
}

