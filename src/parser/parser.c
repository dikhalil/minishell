/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 16:39:30 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/17 17:45:33 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static int	handle_token(t_data *data, t_cmd **current_cmd,
		t_token **current_token)
{
	if ((*current_token)->type == T_WORD)
		handle_word(data, current_cmd, *current_token);
	else if (is_redirection((*current_token)->type))
	{
		if (!handle_redir(data, current_cmd, current_token))
		{
			*current_token = (*current_token)->next;
			return (0);
		}
	}
	else if ((*current_token)->type == T_PIPE)
		return (handle_pipe(data, current_cmd, *current_token));
	return (1);
}

void	parser(t_data *data)
{
	t_cmd	*current_cmd;
	t_token	*current_token;

	if (!data || !data->tokens)
		return ;
	current_cmd = cmd_new(data);
	current_token = data->tokens;
	while (current_token)
	{
		if (!handle_token(data, &current_cmd, &current_token))
		{
			parser_error_handling(data, current_token);
			return ;
		}
		current_token = current_token->next;
	}
	cmd_add_back(&data->cmds, current_cmd);
}
