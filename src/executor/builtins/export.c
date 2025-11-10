/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 12:13:09 by yocto             #+#    #+#             */
/*   Updated: 2025/11/10 07:30:17 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void export_builtin(t_data *data, t_arg *args)
{
    if (!args)
        print_env_sorted(data->env);
    else
    {
        while (args)
        {
            if (!valid_identifier(args->value)){
                write(2, "not a valid identifier\n", 23);
                args = args->next;
                data->last_exit = 2;
                continue;
            }
            else
                add_or_update_env(data, args->value);
            args = args->next;
        }
    }
}

void add_or_update_env(t_data *data, const char *arg)
{
    char    *key;
    char    *value;
    t_env   *current;
    t_env   *new_node;

    key = strdup_until_char(arg, '=');
    if (ft_strchr(arg, '='))
        value = strdup(ft_strchr(arg, '=') + 1);
    else
        value = NULL;
    current = data->env;
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            free(current->value);
            current->value = value;
            free(key);
            return ;
        }
        current = current->next;
    }
    new_node = malloc(sizeof(t_env));
    new_node->key = key;
    new_node->value = value;
    new_node->next = data->env;
    data->env = new_node;
}


char *strdup_until_char(const char *str, char c)
{
    size_t  len;
    char    *dup;

    len = 0;
    while (str[len] && str[len] != c)
        len++;
    dup = malloc(len + 1);
    if (!dup)
        return (NULL);
    strncpy(dup, str, len);
    dup[len] = '\0';
    return (dup);
}

int valid_identifier(const char *str)
{
    int i;

    if (!str || (!ft_isalpha(str[0]) && str[0] != '_'))
        return (0);
    i = 1;
    while (str[i] && str[i] != '=')
    {
        if (!ft_isalnum(str[i]) && str[i] != '_')
            return (0);
        i++;
    }
    return (1);
}

void print_env_sorted(t_env *env)
{
    t_env   *current;
    t_env   *next;
    char    *temp_key;
    char    *temp_value;
    int     swapped;

    if (!env)
        return ;

    swapped = 1;
    while (swapped)
    {
        swapped = 0;
        current = env;
        while (current->next)
        {
            next = current->next;
            if (strcmp(current->key, next->key) > 0)
            {
                temp_key = current->key;
                temp_value = current->value;
                current->key = next->key;
                current->value = next->value;
                next->key = temp_key;
                next->value = temp_value;
                swapped = 1;
            }
            current = current->next;
        }
    }
    current = env;
    while (current)
    {
        if (current->value)
            printf("declare -x %s=\"%s\"\n", current->key, current->value);
        else
            printf("declare -x %s\n", current->key);
        current = current->next;
    }
}
