/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 16:08:49 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/18 16:27:37 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void executer(t_data *data)
{
	//args to array
	//while (data->cmd->args) array[i] = data->cms->args->value ---> data->cmd->args = data->cmd->args->next;
	
	//redir for open files
	//data->cmd->redir  -----> data->cmd->redir->file  -----> data->cmd->redir->type 
	//data->cmd->infile = open(data->cmd->redir->file); or data->cmd->outfile = open(data->cmd->redir->file);
}
