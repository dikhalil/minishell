/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:21:38 by yocto             #+#    #+#             */
/*   Updated: 2025/11/12 17:00:51 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_n_flag(char *s)
{
	int	i;
	int n_counter = 0;
	int e_counter = 0;

	if (!s || s[0] != '-' || (s[1] != 'n' && s[1] != 'e' && s[1] != 'E'))
		return (0);
	i = 2;
	while (s[i])
	{
		if (s[i] != 'n' && s[i] != 'e' && s[i] != 'E')
			return (0);
		if(s[i] == 'n')
			n_counter++;
		else
			e_counter++;
		i++;
	}
	if(e_counter > 0 && n_counter == 0)
		return (0);
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
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
	free(str_exit);
	data->last_exit = 0;
}
