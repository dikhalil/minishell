/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:54:26 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/14 22:26:41 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void free_cmd_arg(t_cmd **cmds)
{
	t_arg *tmp_arg;

	while ((*cmds)->arg)
	{
		tmp_arg = (*cmds)->arg;
		free(tmp_arg->value);
		(*cmds)->arg = (*cmds)->arg->next;
		free(tmp_arg);
	}
}

void free_cmd_redir(t_cmd **cmds)
{
	t_redir *tmp_redir;

	while ((*cmds)->redir)
	{
		tmp_redir = (*cmds)->redir;
		free(tmp_redir->file);
		(*cmds)->redir = (*cmds)->redir->next;
		free(tmp_redir);
	}
}
void	cmd_clear(t_cmd **cmds)
{
	t_cmd	*tmp_cmd;

	if (!cmds)
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

void parser_error_handling(t_cmd **cmds,t_token **tokens, t_token *current_token, int *last_exit)
{
    ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
    if (current_token && current_token->value)
		ft_putstr_fd(current_token->value, 2);
	else
		ft_putstr_fd("newline", 2);
    ft_putendl_fd("'", 2);
    *last_exit = 2;
    token_clear(tokens);
    cmd_clear(cmds);
}