/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 17:21:43 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/20 22:38:43 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void	redir_set_operand(t_data *data, t_cmd **current_cmd, t_redir *redir,
		t_token *token)
{
	if (redir->type != T_HEREDOC)
	{
		redir->file = ft_strdup(token->value);
		if (!redir->file)
		{
			cmd_clear(current_cmd);
			free(redir);
			exit_program(data, ERR_MEM);
		}
	}
	else
	{
		redir->delim = ft_strdup(token->value);
		if (!redir->delim)
		{
			cmd_clear(current_cmd);
			free(redir);
			exit_program(data, ERR_MEM);
		}
	}
	redir->quote = token->quote;
}

t_redir	*redir_new(t_data *data, t_cmd **current_cmd, t_token **current_token)
{
	t_redir	*new;

	new = ft_calloc(1, sizeof(t_redir));
	if (!new)
	{
		cmd_clear(current_cmd);
		exit_program(data, ERR_MEM);
	}
	new->type = (*current_token)->type;
	if (!(*current_token)->next || (*current_token)->next->type != T_WORD)
	{
		cmd_clear(current_cmd);
		free(new);
		return (NULL);
	}
	*current_token = (*current_token)->next;
	redir_set_operand(data, current_cmd, new, *current_token);
	return (new);
}

t_redir	*redir_last(t_redir *head)
{
	if (!head)
		return (NULL);
	while (head->next)
		head = head->next;
	return (head);
}

void	redir_add_back(t_redir **head, t_redir *new)
{
	t_redir	*last;

	if (!head || !new)
		return ;
	if (!*head)
	{
		*head = new;
		return ;
	}
	last = redir_last(*head);
	last->next = new;
	return ;
}

int	handle_redir(t_data *data, t_cmd **current_cmd, t_token **current_token)
{
	t_redir	*current_redir;

	current_redir = redir_new(data, current_cmd, current_token);
	if (!current_redir)
		return (0);
	redir_add_back(&((*current_cmd)->redir), current_redir);
	return (1);
}
