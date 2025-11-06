/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:41:49 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/06 16:43:13 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int g_sig = 0;

int main(int argc, char **argv, char **envp)
{
    t_data data;

    ft_memset(&data, 0, sizeof(t_data));
    init_env(&data, envp);
    data.argv = argv;
    data.argc = argc;
    run_shell(&data);
    env_clear(&data.env);
    return (0);
}
