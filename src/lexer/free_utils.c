/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:53:39 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/14 19:05:24 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void	token_clear(t_token **tokens)
{
	t_token	*tmp;

	if (!tokens)
		return ;
	while (*tokens)
	{
		free((*tokens)->value);
		tmp = *tokens;
		*tokens = (*tokens)->next;
		free(tmp);
	}
	*tokens = NULL;
}

void token_error_handling(t_token **tokens ,int *last_exit)
{
    ft_putendl_fd("minishell: syntax error unclosed quotes", 2);
    *last_exit = 2;
    token_clear(tokens);
}
