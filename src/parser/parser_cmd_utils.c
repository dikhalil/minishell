/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_cmd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 14:38:38 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/16 14:06:11 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

t_cmd *cmd_new(t_data *data)
{
	t_cmd *new = ft_calloc(1, sizeof(t_cmd));
	if (!new)
		exit_program(data, ERR_MEM);
	new->infile = -1;
	new->outfile = -1;
    return (new);
}

t_cmd	*cmd_last(t_cmd *head)
{
	if (!head)
		return (NULL);
	while (head->next)
		head = head->next;
	return (head);
}

void	cmd_add_back(t_cmd **head, t_cmd *new)
{
	t_cmd	*last;

	if (!head || !new)
		return ;
	if (!*head)
	{
		*head = new;
		return ;
	}
	last = cmd_last(*head);
	last->next = new;
}

int	handle_pipe(t_data *data, t_cmd **current_cmd, t_token *current_token)
{
    if (!(*current_cmd)->arg && !(*current_cmd)->redir)
        return (0);
    cmd_add_back(&data->cmds, *current_cmd);
    if (current_token->next)
        *current_cmd = cmd_new(data);
    else
        return (0);
    return (1);
}

