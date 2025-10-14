/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 16:39:30 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/14 22:21:06 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int is_redirection(t_token_type type)
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

t_arg *arg_new(t_cmd **cmds, t_token **tokens, char *value)
{
    t_arg *new = ft_calloc(1, sizeof(t_arg));
    if (!new)
        exit_program(tokens, cmds, ERR_MEM);
    new->value = ft_strdup(value);
    if (!new->value)
        exit_program(tokens, cmds, ERR_MEM);
    return (new);
}
t_redir *redir_new(t_cmd **cmds, t_token **tokens, t_token **current_token)
{
    t_redir *new = ft_calloc(1, sizeof(t_redir));
    if (!new)
        exit_program(tokens, cmds, ERR_MEM);
    new->type = (*current_token)->type;
    if ((*current_token)->next && (*current_token)->next->type == T_WORD)
    {
        *current_token = (*current_token)->next;
        new->file = ft_strdup((*current_token)->value);
        if (!new->file)
            exit_program(tokens, cmds, ERR_MEM);
    }
    else
        return (NULL);
    return (new);
}
t_arg	*arg_last(t_arg *head)
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
t_cmd *cmd_new(t_cmd **cmds, t_token **tokens)
{
    t_cmd *new = ft_calloc(1, sizeof(t_cmd));
    if (!new)
        exit_program(tokens, cmds, ERR_MEM);
    return (new);
}
void parser(t_cmd **cmds, t_token **tokens, int *last_exit)
{
    t_cmd *current_cmd;
    t_token *current_token;
    
    current_cmd = cmd_new(cmds, tokens);
    current_token = *tokens;
    while (current_token)
    {
        if (current_token->type == T_WORD)
            arg_add_back(&current_cmd->arg, arg_new(cmds, tokens, current_token->value));
        else if (is_redirection(current_token->type))
        {
            if (!handle_redir(cmds, tokens, &current_cmd, &current_token))
            {
                parser_error_handling(cmds, tokens, current_token->next, last_exit);
                return ;
            }
        }
        else if (current_token->type == T_PIPE)
        {
            if (!handle_pipe(cmds, tokens, &current_cmd, current_token))
            {
                parser_error_handling(cmds, tokens, current_token, last_exit);
                return ;
            }
        }
        current_token = current_token->next;
    }
	cmd_add_back(cmds, current_cmd);
}

int	handle_pipe(t_cmd **cmds, t_token **tokens, t_cmd **current_cmd, t_token *current_token)
{
    if (!(*current_cmd)->arg && !(*current_cmd)->redir)
        return (0);
    cmd_add_back(cmds, *current_cmd);
    if (current_token->next)
        *current_cmd = cmd_new(cmds, tokens);
    else
        return (0);
    return (1);
}

int handle_redir(t_cmd **cmds, t_token **tokens,t_cmd **current_cmd, t_token **current_token)
{
    t_redir *current_redir;

    current_redir = redir_new(cmds, tokens, current_token);
    if (!current_redir)
        return (0);
    redir_add_back(&((*current_cmd)->redir), current_redir);   
    return (1);
}

