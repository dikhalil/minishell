/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:21:38 by yocto             #+#    #+#             */
/*   Updated: 2025/11/03 17:21:39 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	echo_builtin(t_data *data, t_arg *args)
{
	int		newline;
	char	*str_exit;

	newline = 1;
	str_exit = ft_itoa(data->last_exit);
	if (args && ft_strcmp(args->value, "-n") == 0)
	{
		newline = 0;
		args = args->next;
	}
	if (args && ft_strcmp(args->value, "$?") == 0)
	{
		write(STDOUT_FILENO, str_exit, ft_strlen(str_exit));
		args = args->next;
	}
	while (args)
	{
		write(STDOUT_FILENO, args->value, ft_strlen(args->value));
		args = args->next;
		if (args)
			write(STDOUT_FILENO, " ", 1);
	}
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
	free(str_exit);
}
