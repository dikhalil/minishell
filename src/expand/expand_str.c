/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_str.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 22:27:04 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/20 10:12:27 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void	expand_str(t_data *data, char **str)
{
	char	*cur_val;
	char	*old_val;
	char	*new_val;
	int		i;

	i = 0;
	new_val = NULL;
	cur_val = *str;
	while (cur_val[i])
	{
		if (cur_val[i] == '$')
		{
			i++;
			new_val = str_join_free(new_val,
					extract_value(data, extract_key(data,
							cur_val, &i)));
		}
		else
			new_val = str_join_chr(new_val, cur_val[i++]);
		if (!new_val)
			exit_program(data, ERR_MEM);
	}
	old_val = *str;
	*str = new_val;
	free(old_val);
}
