/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 12:13:09 by yocto             #+#    #+#             */
/*   Updated: 2025/11/18 18:38:16 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_export_error(t_arg **args, t_data *data, int *flag)
{
	ft_putstr_fd((*args)->value, STDERR_FILENO);
	ft_putendl_fd(" :not a valid identifier", STDERR_FILENO);
	*args = (*args)->next;
	data->last_exit = 1;
	*flag = 1;
}

static void	process_export_args(t_data *data, t_arg *args, int *flag)
{
	while (args)
	{
		if (!valid_identifier(args->value))
			handle_export_error(&args, data, flag);
		else
		{
			add_or_update_env(data, args->value);
			args = args->next;
		}
	}
}

void	export_builtin(t_data *data, t_arg *args)
{
	int	flag;

	flag = 0;
	if (!args)
	{
		print_env_sorted(data->env);
		data->last_exit = 0;
	}
	else
		process_export_args(data, args, &flag);
	if (!flag)
		data->last_exit = 0;
}
