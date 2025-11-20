/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 12:13:09 by yocto             #+#    #+#             */
/*   Updated: 2025/11/20 11:37:39 by dikhalil         ###   ########.fr       */
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

t_arg	*skip_empty_args(t_arg *args)
{
	while (args && args->value && args->value[0] == '\0' && args->expanded)
		args = args->next;
	return (args);
}

static int	handle_valid_export(t_data *data, t_arg **args, int counter)
{
	add_or_update_env(data, (*args)->value);
	*args = (*args)->next;
	counter++;
	if (!(*args) && counter == 1)
		counter++;
	return (counter);
}

static int	process_export_args(t_data *data, t_arg *args, int *flag)
{
	int	counter;

	counter = 0;
	while (args)
	{
		args = skip_empty_args(args);
		if (!args)
		{
			counter++;
			return (counter);
		}
		if (!valid_identifier(args->value))
		{
			handle_export_error(&args, data, flag);
			return (0);
		}
		else
			counter = handle_valid_export(data, &args, counter);
	}
	return (counter);
}

void	export_builtin(t_data *data, t_arg *args)
{
	int	flag;
	int	code;

	code = 0;
	flag = 0;
	if (!args)
	{
		print_env_sorted(data->env);
		data->last_exit = 0;
	}
	else
		code = process_export_args(data, args, &flag);
	if (code == 1)
	{
		print_env_sorted(data->env);
		data->last_exit = 0;
	}
	if (!flag)
		data->last_exit = 0;
}
