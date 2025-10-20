/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_str.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 22:27:04 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/20 22:35:13 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void expand_str(t_data *data, char **str)
{
    char *cur_val;
    char *old_val;
    char *new_val;
    int i;

    i = 0;
    new_val = NULL;
    cur_val= *str;
    while (cur_val[i])
    { 
        if (cur_val[i] == '$')
        {
            i++;
            new_val = str_join_free(new_val, 
                extract_value(data, extract_key(data, cur_val, &i)));
        }
        else
            new_val = str_join_chr(new_val, cur_val[i++]);
        if (!new_val)
            exit_program(data, ERR_MEM);
    }
    old_val = *str;
    *str = new_val;
    free(old_val);
}
void expand_single_arg(t_data *data, t_arg *arg)
{
    if (!arg || !arg->value || arg->quote == SINGLE_QUOTE)
        return;
    if (ft_strchr(arg->value, '$'))
        expand_str(data, &arg->value);
}

static void create_new_args(t_arg *prev, char **split_arg, int start)
{
    t_arg *new;
    int i;

    i = start;
    while (split_arg[i])
    {
        new = ft_calloc(1, sizeof(t_arg));
        if (!new)
            return;
        new->value = ft_strdup(split_arg[i]);
        new->quote = prev->quote;
        new->next = prev->next;
        prev->next = new;
        prev = new;
        i++;
    }
}

void split_arg_spaces(t_arg *arg)
{
    char **split_arg;

    if (!arg || !arg->value || arg->quote != NONE || !ft_strchr(arg->value, ' '))
        return;
    split_arg = ft_split(arg->value, ' ');
    if (!split_arg)
        return;
    free(arg->value);
    arg->value = ft_strdup(split_arg[0]);
    create_new_args(arg, split_arg, 1);
    free_split(split_arg);
}