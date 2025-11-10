/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:21:38 by yocto             #+#    #+#             */
/*   Updated: 2025/11/10 17:59:22 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_n_flag(char *s)
{
	int	i;

	if (!s || s[0] != '-' || s[1] != 'n')
		return (0);
	i = 2;
	while (s[i])
	{
		if (s[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

void	echo_builtin(t_data *data, t_arg *args)
{
	int		newline;
	char	*str_exit;
	// int code;

	// code = 1;
	newline = 1;
	str_exit = ft_itoa(data->last_exit);
	while (args && is_n_flag(args -> value))
	{
		newline = 0;
		args = args->next;
	}
	while (args)
	{
		write(STDOUT_FILENO, args->value, ft_strlen(args->value));
		args = args->next;
		if (args)
			write(STDOUT_FILENO, " ", 1);
	}
	if (newline && data->cmds->outfile == STDOUT_FILENO)
		write(STDOUT_FILENO, "\n", 1);
	free(str_exit);
	data->last_exit = 0;
}
