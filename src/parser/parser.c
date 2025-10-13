/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 16:39:30 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/13 21:00:03 by dikhalil         ###   ########.fr       */
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

t_token	*redir_last(t_redir *head)
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
}

void parser(t_cmd **cmd, t_token **tokens, int *last_exit)
{
    t_cmd *current_cmd;
    t_redir *current_redir;
    t_token *current_token;
    int i;
    
    current_cmd = ft_calloc(1, sizeof(t_cmd));
    if (!current_cmd)
        //error
    //malloc current_cmd->argv
    *cmd = current_cmd;
    current_token = *tokens;
    current_redir = NULL;
    i = 0;
    while (current_token)
    {
        if (current_token->type == T_WORD)
        {
            current_cmd->argv[i++] = ft_strdup(current_token->value);
            if (!current_cmd->argv[i])
                //error
        }
        else if (is_redir(current_token->type))
        {
            current_redir = ft_calloc(1, sizeof(t_redir));
            if (!current_redir)
                //error
            current_redir->type = current_token->type;
            if (current_token->next && current_token->next->type == T_WORD)
            {
                current_token = current_token->next;
                current_redir->file = ft_strdup(current_token->value);
            }
            else
                //ERROR
            redir_add_back(&current_cmd->redir, current_redir);
        }
        else if (current_token->type == PIPE)
        {
            if (current_cmd->argv || current_cmd->redir)
            {
                if (current_token->next && (current_token->next->type == T_WORD || is_redir(current_token->next->type)) )
                {
                    cmd_add_back(cmd, current_cmd);
                    //create new cmd
                    current_cmd = ft_calloc(1, sizeof(t_cmd));
                    if (!current_cmd)
                        // handle error
                    //malloc current_cmd->argv
                    i = 0;
                    current_redir = NULL;
                }
            }
            else
                //error
        }
        current_token = current_token->next;
    }
    
}

