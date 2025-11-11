/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_free_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:54:26 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/11 23:14:03 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void	free_cmd_arg(t_cmd **cmds)
{
	t_arg	*tmp_arg;

	if (!(*cmds)->arg)
		return ;
	while ((*cmds)->arg)
	{
		tmp_arg = (*cmds)->arg;
		free(tmp_arg->value);
		(*cmds)->arg = (*cmds)->arg->next;
		free(tmp_arg);
	}
}

static void	free_cmd_redir(t_cmd **cmds)
{
	t_redir	*tmp_redir;

	if (!(*cmds)->redir)
		return ;
	while ((*cmds)->redir)
	{
		tmp_redir = (*cmds)->redir;
		if (tmp_redir->file)
			free(tmp_redir->file);
		if (tmp_redir->delim)
			free(tmp_redir->delim);
		(*cmds)->redir = (*cmds)->redir->next;
		free(tmp_redir);
	}
}

void	cmd_clear(t_cmd **cmds)
{
	t_cmd	*tmp_cmd;

	if (!cmds || !*cmds)
		return ;
	while (*cmds)
	{
		free_cmd_arg(cmds);
		free_cmd_redir(cmds);
		tmp_cmd = *cmds;
		*cmds = (*cmds)->next;
		free(tmp_cmd);
	}
	*cmds = NULL;
}

void	parser_error_handling(t_data *data, t_token *current_token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	if (current_token && current_token->value)
		ft_putstr_fd(current_token->value, 2);
	else
		ft_putstr_fd("newline", 2);
	ft_putendl_fd("'", 2);
	data->last_exit = 2;
	free_all(data);
}
