/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 20:57:51 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/15 22:47:25 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void exit_program(t_env **env, t_token **tokens, t_cmd **cmds, int status)
{
    if (env)
        env_clear(env);
    if (tokens)
        token_clear(tokens);
    if (cmds)
        cmd_clear(cmds);
    exit(status);
}
