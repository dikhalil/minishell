/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 20:56:46 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/13 16:57:37 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void	token_clear(t_token **token, void (*del)(char *))
{
	t_token	*tmp;

	if (!token || !del)
		return ;
	while (*token)
	{
		del((*token)->value);
		tmp = *token;
		*token = (*token)->next;
		free(tmp);
	}
	*token = NULL;
}

void del(char *value)
{
    free(value);
}

void	cmd_clear(t_cmd **cmd, void (*del)(char *))
{
	t_cmd	*tmp_cmd;
	int i;
	t_redir *tmp_redir;

	i = 0;
	if (!cmd || !del)
		return ;
	while (*cmd)
	{
		if ((*cmd)->argv)
		{
			i = 0;
			while ((*cmd)->argv[i])
				del((*cmd)->argv[i++]);
			free((*cmd)->argv);
		}
		while ((*cmd)->redir)
		{
			tmp_redir = (*cmd)->redir;
			del(tmp_redir->file);
			(*cmd)->redir = (*cmd)->redir->next;
			free(tmp_redir);
		}
		tmp_cmd = *cmd;
		*cmd = (*cmd)->next;
		free(tmp_cmd);
	}
	*cmd = NULL;
}
