/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 01:20:24 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/12 22:25:09 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <minishell.h>

static t_quote_type	get_quote_type(char c)
{
	if (c == '"')
		return (DOUBLE_QUOTE);
	else if (c == '\'')
		return (SINGLE_QUOTE);
	return (NONE);
}

static int	skip_quoted(char *str, int i, char quote_chr)
{
	while (str[i] && str[i] != quote_chr)
		i++;
	if (!str[i])
		return (-1);
	return (i + 1);
}

static int	skip_unquoted(char *str, int i)
{
	while (str[i] && !is_space(str[i]) && !is_redir(str[i])
		&& get_quote_type(str[i]) == NONE)
		i++;
	return (i);
}

static char	*extract_word(t_data *data, char *str, int *i, t_quote_type *quote_type)
{
	int		start;
	char	*part;
	char	quote_chr;

	quote_chr = '\0';
	if (get_quote_type(str[*i]) != NONE)
	{
		*quote_type = get_quote_type(str[*i]);
		quote_chr = str[*i];
		(*i)++;
		start = *i;
		*i = skip_quoted(str, *i, quote_chr);
		if (*i == -1)
			return (NULL);
		part = ft_substr(str, start, *i - start - 1);
	}
	else
	{
		*quote_type = NONE;
		start = *i;
		*i = skip_unquoted(str, *i);
		part = ft_substr(str, start, *i - start);
	}
	if (!part)
		exit_program(data, ERR_MEM);
	return (part);
}

int	add_word(t_data *data, char *str)
{
	int				idx;
	char			*word;
	char			*part;
	char			**splited;
	int				j;
	t_quote_type	quote_type;

	idx = 0;
	word = NULL;
	splited = NULL;
	quote_type = NONE;

	while (str[idx] && !is_space(str[idx]) && !is_redir(str[idx]))
	{
		part = extract_word(data, str, &idx, &quote_type);
		if (!part)
		{
			if (splited)
				free_split(splited);
			if (word)
				free(word);
			return (-1);
		}
		if (quote_type != SINGLE_QUOTE && ft_strchr(part, '$'))
			expand_str(data, &part);
		if (part && part[0] != '\0')
		{
			if (quote_type == NONE && ft_strchr(part, ' '))
			{
				splited = ft_split(part, ' ');
				if (part)
					free(part);
				part = NULL;
				if (!splited)
				{
					if (word)
						free(word);
					exit_program(data, ERR_MEM);
				}
			}
			if (!word)
			{
				if (splited)
				{
					j = 0;
					while (splited[j] && splited[j][0] != '\0')
					{
						if (splited[j + 1] != NULL)
						{
							word = ft_strdup(splited[j]);
							if (!word)
							{
								free_split(splited);
								exit_program(data, ERR_MEM);
							}
							add_token(data, word, T_WORD, NONE);
							if (word)
								free(word);
							word = NULL;
						}
						else
						{
							word = ft_strdup(splited[j]);
							if (!word)
							{
								free_split(splited);
								exit_program(data, ERR_MEM);
							}
						}
						j++;
					}
					free_split(splited);
					splited = NULL;
				}
				else
				{
					word = ft_strdup(part);
					free(part);
					part = NULL;
				}
			}
			else
			{
				if (splited)
				{
					j = 0;
					while (splited[j] && splited[j][0] != '\0')
					{
						if (splited[j + 1] != NULL)
						{
							part = ft_strdup(splited[j]);
							if (!part)
							{
								free_split(splited);
								free(word);
								word = NULL;
								splited = NULL;
								exit_program(data, ERR_MEM);
							}
							word = str_join_free(word, part);
							part = NULL;
							if (!word)
							{
								free_split(splited);
								exit_program(data, ERR_MEM);
							}
							add_token(data, word, T_WORD, NONE);
							free(word);
							word = NULL;
						}
						else
						{
							part = ft_strdup(splited[j]);
							if (!part)
							{
								free_split(splited);
								free(word);
								word = NULL;
								splited = NULL;
								exit_program(data, ERR_MEM);
							}
							word = str_join_free(word, part);
							part = NULL;
							if (!word)
							{
								free_split(splited);
								exit_program(data, ERR_MEM);
							}
						}
					
						j++;
					}
					free_split(splited);
					splited = NULL;
				}
				else
				{
					word = str_join_free(word, part);
					part = NULL; 
					if (!word)
					{
						exit_program(data, ERR_MEM);
					}
				}
			}
		}
		else  
		{
			if(part)
			{	
				free(part);
				part = NULL;
			}
		}
	}
	if (word && word[0] != '\0')
		add_token(data, word, T_WORD, quote_type);
	if (splited)
		free_split(splited);
	if (word)
		free(word);
	if (part)
		free(part);
	return (idx);
}