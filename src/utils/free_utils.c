/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 20:56:46 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/12 21:01:59 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    value = NULL;
}
