/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:24:38 by yocto             #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2025/11/06 18:16:13 by dikhalil         ###   ########.fr       */
=======
/*   Updated: 2025/11/07 14:07:08 by yocto            ###   ########.fr       */
>>>>>>> fa4ae4e (i change but it doesn't change :()
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int ft_isnumber(const char *s)
{
<<<<<<< HEAD
    free_all(data);
    exit(exit_code);
}
=======
    int i = 0;

    if (!s || !*s)
        return 0;
    if (s[0] == '+' || s[0] == '-')
        i++;
    if (!s[i])
        return 0;
    while (s[i])
    {
        if (!ft_isdigit((unsigned char)s[i]))
            return 0;
        i++;
    }
    return 1;
}

void	exit_builtin(t_data *data, t_arg *arg)
{
	write(1, "exit\n", 5);
    if (arg && !ft_isnumber(arg->value))
	{
		write(2, "exit: ", 6);
		write(2, arg->value, ft_strlen(arg->value));
		write(2, ": numeric argument required\n", 29);
		free_all(data);
		exit(2);
	}
	else if (arg && arg->next)
	{
		write(2, "exit: too many arguments\n", 25);
		data->last_exit = 1;
		return;
	}
	else if (arg)
	{
		int code = ft_atoi(arg->value);
		free_all(data);
		exit(code);
	}
	free_all(data);
	exit(0);
}

>>>>>>> fa4ae4e (i change but it doesn't change :()
