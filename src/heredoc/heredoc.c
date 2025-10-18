/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 17:46:42 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/18 14:16:59 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void heredoc(t_data *data)
{
    t_cmd *cmd;
    t_redir *redir;

    cmd = data->cmds;
    while (cmd)
    {
        redir = cmd->redir;
        while (redir)
        {
            if (redir->type == T_HEREDOC)
                handle_heredoc(data, cmd, redir);
            redir = redir->next;
        }
        cmd = cmd->next;
    }
}
