/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:24:38 by yocto             #+#    #+#             */
/*   Updated: 2025/11/20 10:08:45 by dikhalil         ###   ########.fr       */
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

static int	check_overflow(const char *num)
{
	char	*max;
	char	*min;
	int		sign;
	int		len;

	max = "9223372036854775807";
	min = "9223372036854775808";
	sign = 1;
	if (*num == '+' || *num == '-')
	{
		if (*num == '-')
			sign = -1;
		num++;
	}
	while (*num == '0')
		num++;
	len = ft_strlen(num);
	if (len > 19)
		return (1);
	if (len < 19)
		return (0);
	if (sign == 1)
		return (ft_strcmp(num, max) > 0);
	else
		return (ft_strcmp(num, min) > 0);
}

static int	exit_process_arg(t_data *data, t_arg *arg, int is_child)
{
	long long	num;

	if (!ft_isnumber(arg->value) || check_overflow(arg->value))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(arg->value, 2);
		ft_putendl_fd(": numeric argument required", 2);
		data->last_exit = 2;
		return (0);
	}
	num = ft_atoll(arg->value);
	if (arg->next)
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		data->last_exit = 1;
		if (!is_child)
			return (1);
	}
	else
		data->last_exit = (int)(num & 0xFF);
	return (0);
}

void	exit_builtin(t_data *data, t_arg *arg, int is_child, char **envp)
{
	int	ret;

	if (!is_child)
		ft_putendl_fd("exit", 1);
	ret = 0;
	if (arg)
		ret = exit_process_arg(data, arg, is_child);
	if (ret == 1)
		return ;
	ex_free_split(envp);
	exit_program_v2(data, data->last_exit);
}
