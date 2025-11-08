/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 15:06:11 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/08 14:47:18 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void	expand_all_args(t_data *data, t_cmd *cmd)
{
	t_arg	*arg;
	t_arg	*prev;
	t_arg	*next;

	arg = cmd->arg;
	prev = NULL;
	next = NULL;
	while (arg)
	{
		next = arg->next;
		if (!expand_single_arg(data, cmd, arg, prev))
		{
			split_arg_spaces(arg);
			prev = arg;
		}
		arg = next;
	}
}

static void	expand_redir(t_data *data, t_redir *redir)
{
	while (redir)
	{
		if (redir->quote != SINGLE_QUOTE && redir->file
			&& redir->type != T_HEREDOC && ft_strchr(redir->file, '$'))
			expand_str(data, &redir->file);
		redir = redir->next;
	}
}

void	expand(t_data *data)
{
	t_cmd	*cmd;

	cmd = data->cmds;
	while (cmd)
	{
		expand_all_args(data, cmd);
		expand_redir(data, cmd->redir);
		cmd = cmd->next;
	}
}
