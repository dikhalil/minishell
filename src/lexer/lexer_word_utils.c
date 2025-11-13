/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 01:20:24 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/13 14:21:47 by dikhalil         ###   ########.fr       */
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

static void	process_split_array(t_data *data, char **splited, char **word)
{
    int		j;
    char	*temp;
    t_token *last;

    j = 0;
    
    if (!splited[0])
    {
        temp = str_join_free(*word,ft_strdup(""));
        if (!temp)
        {
            free_split(splited);
            if (*word)
                free(*word);
            word = NULL;
            exit_program(data, ERR_MEM);
        }
        *word = temp;
    }
    while (splited[j])
    {
        if (*word)
            add_token(data, *word, T_WORD, NONE); 
        temp = ft_strdup(splited[j]);
        if (!temp)
        {
            free_split(splited);
            if (*word)
                free(*word);
            word = NULL;
            exit_program(data, ERR_MEM);
        }
        add_token(data, temp, T_WORD, NONE); 
        last = token_last(data->tokens);
        if (last)
            last->expanded = 1;
        free(temp);
        free(*word);
        *word = NULL;
        j++;
    }
}

static void	handle_part(t_data *data, char *part, char **word, t_quote_type quote_type)
{
    char	**splited;

    splited = NULL;
    if (part && quote_type == NONE && ft_strchr(part, ' '))
    {
        splited = ft_split(part, ' ');
        free(part);
        part = NULL;
        if (!splited)
        {
            if (*word)
                free(*word);
            exit_program(data, ERR_MEM);
        }
        process_split_array(data, splited, word);
        free_split(splited);
    }
    else
    {
        *word = str_join_free(*word, part);
        if (!*word)
            exit_program(data, ERR_MEM);
    }
}


int	add_word(t_data *data, char *str)
{
    int				idx;
    char			*word;
    char			*part;
    int				expanded;
    t_quote_type	quote_type;
    t_quote_type	last_quote;
    t_token *last;

    idx = 0;
    word = NULL;
    last_quote = NONE;
    expanded = 0;
    while (str[idx] && !is_space(str[idx]) && !is_redir(str[idx]))
    {
        part = extract_word(data, str, &idx, &quote_type);
        if (!part)
        {
            if (word)
                free(word);
            return (-1);
        }
        if (quote_type != SINGLE_QUOTE && ft_strchr(part, '$'))
            expand_str(data, &part, &expanded);
        if (part)
        {
            handle_part(data, part, &word, quote_type);
            last_quote = quote_type;
        }
    }
    if (word)
    {
        add_token(data, word, T_WORD, last_quote);
        last = token_last(data->tokens);
        if (last)
            last->expanded = expanded;
    }
    if (word)
        free(word);
    return (idx);
}
