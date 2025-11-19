/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word_split.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 22:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/19 21:54:49 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

void	process_split_array(t_data *data, char **splited, char **word)
{
	int		j;
	t_token	*last;

	j = 0;
	while (splited[j])
	{
		if (splited[j][0] == '\0')
		{
			j++;
			continue ;
		}
		*word = ft_strdup(splited[j]);
		if (!*word)
			free_and_exit(data, splited, word);
		add_token(data, *word, T_WORD, NONE);
		last = token_last(data->tokens);
		if (last)
			last->expanded = 1;
		free(*word);
		*word = NULL;
		j++;
	}
}

int	check_split_content(char **splited)
{
	int	i;

	i = 0;
	while (splited[i])
	{
		if (splited[i][0] != '\0')
			return (1);
		i++;
	}
	return (0);
}

void	extract_and_join_prefix(t_data *data, char *part,
		char **word, int *first_space)
{
	char	*prefix;

	*first_space = 0;
	while (part[*first_space] && part[*first_space] != ' ')
		(*first_space)++;
	if (*first_space > 0 && part[*first_space] == ' ')
	{
		prefix = ft_substr(part, 0, *first_space);
		if (!prefix)
			exit_program(data, ERR_MEM);
		*word = str_join_free(*word, prefix);
		if (!*word)
			exit_program(data, ERR_MEM);
	}
}

void	handle_split_part(t_data *data, char *part, char **word)
{
	char	**splited;
	int		first_space;

	extract_and_join_prefix(data, part, word, &first_space);
	splited = ft_split(part + first_space, ' ');
	free(part);
	if (!splited)
		free_and_exit(data, NULL, word);
	if (!check_split_content(splited))
	{
		free_split(splited);
		return ;
	}
	if (*word)
	{
		add_token(data, *word, T_WORD, NONE);
		free(*word);
		*word = NULL;
	}
	process_split_array(data, splited, word);
	free_split(splited);
}
