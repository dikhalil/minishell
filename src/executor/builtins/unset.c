/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:06:31 by yocto             #+#    #+#             */
/*   Updated: 2025/11/12 18:17:14 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	unset_builtin(t_data *data, t_arg *args)
{
	t_env	*current;
	t_env	*prev;
	t_env	*to_delete;

	while (args)
	{
		current = data->env;
		prev = NULL;
		while (current)
		{
			if (ft_strcmp(current->key, args->value) == 0)
			{
				to_delete = current;
				if (prev)
					prev->next = current->next;
				else
					data->env = current->next;
				free(to_delete->key);
				free(to_delete->value);
				free(to_delete);
				break ;
			}
			prev = current;
			current = current->next;
		}
		args = args->next;
	}
}
