/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:24:38 by yocto             #+#    #+#             */
/*   Updated: 2025/11/07 19:36:13 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isnumber(const char *s)
{
	int	i;

	i = 0;
	if (!s || !*s)
		return (0);
	if (s[0] == '+' || s[0] == '-')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

void	exit_builtin(t_data *data, t_arg *arg, int is_child)
{
	int	code;

	code = 0;
	if (!is_child)
		ft_putendl_fd("exit", 1);
	if (arg && !ft_isnumber(arg->value))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(arg->value, 2);
		ft_putendl_fd(": numeric argument required", 2);
		code = 2;
	}
	else if (arg && arg->next)
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		code = 1;
		data->last_exit = 1;
		if (!is_child)
			return ;
	}
	else if (arg)
		code = ft_atoi(arg->value) & 0xFF;
	data->last_exit = code;
	if (is_child)
	{
		free_all(data);
		exit(code);
	}
	else
		exit_program_v2(data, code);
}
