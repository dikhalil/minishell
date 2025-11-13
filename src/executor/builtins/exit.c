/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:24:38 by yocto             #+#    #+#             */
/*   Updated: 2025/11/13 14:30:52 by dikhalil         ###   ########.fr       */
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

void	exit_builtin(t_data *data, t_arg *arg, int is_child, char **envp)
{
	long long num;

	if (!is_child)
		ft_putendl_fd("exit", 1);
	data->last_exit = 0;
	if (arg)
	{
		if (!ft_isnumber(arg->value))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(arg->value, 2);
			ft_putendl_fd(": numeric argument required", 2);
			data->last_exit  = 2;
		}
		else 
		{
			num = ft_atoll(arg->value);
			if ((num == LLONG_MAX && arg->value[0] != '-') ||
    			(num == LLONG_MIN && arg->value[0] == '-')
				|| (num >= LLONG_MAX) || (num <= LLONG_MIN))
			{			
				ft_putstr_fd("minishell: exit: ", 2);
				ft_putstr_fd(arg->value, 2);
				ft_putendl_fd(": numeric argument required", 2);
				data->last_exit = 2;
			}
			if (arg->next)
			{
				ft_putendl_fd("minishell: exit: too many arguments", 2);
				data->last_exit = 1;
				if (!is_child)
					return;
			}
			else if (data->last_exit == 0)
				data->last_exit = (int)(num & 0xFF);
		}
	}
	ex_free_split(envp);
	exit_program_v2(data, data->last_exit);
}
