/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 18:01:04 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/17 21:13:29 by dikhalil         ###   ########.fr       */
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

char *extract_value(t_data *data, char *key)
{
    t_env *env;
    char *value;
    int num;
    
    env = data->env;
    if (!ft_strcmp(key, "$"))
        value = ft_strdup(key);
    else if (!ft_strcmp(key, "?"))
        value = ft_itoa(data->last_exit);
    else if (is_number(key))
    {
        num = ft_atoi(key);
        if (num >= 0 && num < data->argc)
            value = ft_strdup(data->argv[num]);
        else
            value = ft_strdup("");
    }
    else
    {
        while (env)
        {
            if (ft_strcmp(env->key, key) == 0)
                break;
            env = env->next;
        }
        if (!env)
            value = ft_strdup("");
        else
            value = ft_strdup(env->value);
    }
    free(key);
    if (!value)
        exit_program(data, ERR_MEM);
    return (value);
}
