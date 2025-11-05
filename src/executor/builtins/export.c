/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 12:13:09 by yocto             #+#    #+#             */
/*   Updated: 2025/11/05 12:16:48 by yocto            ###   ########.fr       */
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
            if (!valid_identifier(args->value))
                print_error("not a valid identifier");
            else
                add_or_update_env(data, args->value);
            args = args->next;
        }
    }
}
