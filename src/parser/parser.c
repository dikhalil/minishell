/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 16:39:30 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/14 03:23:04 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int is_redir(t_token_type type)
{
    return (type == T_IN_REDIR || type == T_OUT_REDIR ||
            type == T_APPEND || type == T_HEREDOC);
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

t_redir	*redir_last(t_redir *head)
{
	if (!head)
		return (NULL);
	while (head->next)
		head = head->next;
	return (head);
}

int	redir_add_back(t_redir **head, t_redir *new)
{
	t_redir	*last;

	if (!head || !new)
		return (0);
	if (!*head)
	{
		*head = new;
		return (1);
	}
	last = redir_last(*head);
	last->next = new;
    return (1);
}

t_arg *arg_new(char *value)
{
    t_arg *new = ft_calloc(1, sizeof(t_arg));
    if (!new)
        return (NULL);
    new->value = ft_strdup(value);
    if (!new->value)
        return (NULL);
    return (new);
}
t_redir *redir_new(t_token **token)
{
    t_redir *new = ft_calloc(1, sizeof(t_redir));
    if (!new)
        return (NULL);
    new->type = (*token)->type;
    if ((*token)->next && (*token)->next->type == T_WORD)
    {
        *token = (*token)->next;
        new->file = ft_strdup((*token)->value);
        if (!new->file)
            return (NULL);
    }
    else
        return (NULL);
    return (new);
}
t_ard	*arg_last(t_arg *head)
{
	if (!head)
		return (NULL);
	while (head->next)
		head = head->next;
	return (head);
}
void arg_add_back(t_arg **head, t_arg *new)
{
    t_arg *last;

    if (!*head)
    {
        *head = new;
        return;
    }
    last = arg_last(*head);
	last->next = new;
}
t_arg *cmd_new()
{
    t_cmd *new = ft_calloc(1, sizeof(t_cmd));
    if (!new)
        return (NULL);//error
    return (new);
}
void parser(t_cmd **cmd, t_token **tokens, int *last_exit)
{
    t_cmd *current_cmd;
    t_token *current_token;
    
    current_cmd = cmd_new();
    current_token = *tokens;
    while (current_token)
    {
        if (current_token->type == T_WORD)
            arg_add_back(&current_cmd->arg, arg_new(current_token->value));
        else if (is_redir(current_token->type))
        {
            if (!redir_add_back(&current_cmd->redir, redir_new(&current_token)))
                //error in redir
        }
        else if (current_token->type == PIPE)
        {
            if (!handle_pipe(cmd, &current_cmd, current_token))
                //error in pipe
        }
        current_token = current_token->next;
    }
	cmd_add_back(cmd, current_cmd);
}

int	handle_pipe(t_cmd **cmd, t_cmd **current_cmd, t_token *current_token)
{
    if (!(*current_cmd)->arg && !(*current_cmd)->redir)
        return (0);
    cmd_add_back(cmd, *current_cmd);
    if (current_token->next)
        *current_cmd = cmd_new();
    return (1);
}

