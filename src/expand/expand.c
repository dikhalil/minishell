/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 15:06:11 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/18 14:18:59 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void expand_str(t_data *data, char **str)
{
    char *cur_val;
    char *old_val;
    char *new_val;
    int i;

    if (!*str)
        return ;
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

void expand(t_data *data)
{
    t_cmd *cmd;
    t_arg *arg;
    t_redir *redir;
    t_arg *prev;
    t_arg *new;
    int i;
    char **split_arg;

    cmd = data->cmds;
    while (cmd)
    {
        arg = cmd->arg;
        redir = cmd->redir;
        while (arg)
        {
            if (arg->value && arg->quote != SINGLE_QUOTE)
            {
                if (ft_strchr(arg->value, '$'))
                {
                    expand_str(data, &arg->value); 
                    if (arg->quote == NONE &&
                        (ft_strchr(arg->value, ' ')))
                    {
                        split_arg = ft_split(arg->value, ' ');
                        if (!split_arg)
                            exit_program(data,ERR_MEM);
                        free(arg->value);
                        arg->value = ft_strdup(split_arg[0]);
                        if (!arg->value)
                            exit_program(data, ERR_MEM);
                        i = 1;
                        prev = arg;
                        while (split_arg[i])
                        {
                            new = ft_calloc(1, sizeof(t_arg));
                            if (!new)
                                exit_program(data, ERR_MEM);
                            new->value = ft_strdup(split_arg[i]);
                            if (!new->value)
                                exit_program(data, ERR_MEM);
                            new->quote = prev->quote;
                            new->next = prev->next;
                            prev->next = new;
                            prev = new;
                            i++;
                        }
                        free_split(split_arg);
                    }
                }
            }                   
            arg = arg->next;
        }
        while (redir)
        {
            if (redir->quote != SINGLE_QUOTE)
            {
                if (redir->file && redir->type != T_HEREDOC)
                {
                    if (ft_strchr(redir->file, '$'))
                        expand_str(data, &redir->file);
                }
            }
            redir = redir->next;
        }
        cmd = cmd->next;
    }
    
}
