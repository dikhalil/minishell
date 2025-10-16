/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 17:21:43 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/16 14:01:42 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int is_redirection(t_token_type type)
{
    return (type == T_IN_REDIR || type == T_OUT_REDIR ||
            type == T_APPEND || type == T_HEREDOC);
}

t_redir *redir_new(t_data *data, t_token **current_token)
{
    t_redir *new = ft_calloc(1, sizeof(t_redir));
    if (!new)
        exit_program(data, ERR_MEM);
    new->type = (*current_token)->type;
    if ((*current_token)->next && (*current_token)->next->type == T_WORD)
    {
        *current_token = (*current_token)->next;
        new->file = ft_strdup((*current_token)->value);
        new->quote = (*current_token)->quote;
        if (!new->file)
            exit_program(data, ERR_MEM);
    }
    else
        return (NULL);
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

int handle_redir(t_data *data, t_cmd **current_cmd, t_token **current_token)
{
    t_redir *current_redir;

    current_redir = redir_new(data, current_token);
    if (!current_redir)
        return (0);
    redir_add_back(&((*current_cmd)->redir), current_redir);   
    return (1);
}
