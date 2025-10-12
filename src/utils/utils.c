/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 20:57:51 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/12 21:02:25 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void error_handling(t_token **token ,int *last_exit, char *msg, int status)
{
    if (msg)
        ft_putendl_fd(msg, 2);
    *last_exit = status;
    token_clear(token, del);
}

void exit_program(t_token **token, int status)
{
    token_clear(token, del);
    exit(status);
}