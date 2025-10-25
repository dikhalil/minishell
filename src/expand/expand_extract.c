/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_extract.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 18:01:04 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/20 22:32:29 by dikhalil         ###   ########.fr       */
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

    if (!ft_strcmp(key, "$"))
        return (ft_strdup(key));
    else if (!ft_strcmp(key, "?"))
        return (ft_itoa(data->last_exit));
    else if (is_number(key))
    {
        num = ft_atoi(key);
        if (num >= 0 && num < data->argc)
            return (ft_strdup(data->argv[num]));
        else
            return (ft_strdup(""));
    }
    return (NULL);
}

static char *extract_env(t_data *data, char *key)
{
    t_env *env = data->env;

    while (env)
    {
        if (ft_strcmp(env->key, key) == 0)
            return (ft_strdup(env->value));
        env = env->next;
    }
    return (ft_strdup(""));
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

