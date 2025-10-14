/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 15:13:07 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/14 22:15:20 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int add_token(t_token **tokens, char *value, t_token_type type, t_quote_type quote)
{
    t_token *new;

    new = malloc(sizeof(t_token));
    if (!new)
        exit_program(tokens, NULL,  ERR_MEM);
    new->value = ft_strdup(value);
    if (!new->value)
        exit_program(tokens, NULL, ERR_MEM);
    new->quote = quote;
    new->type = type;
    new->next = NULL;
    token_add_back(tokens, new);
    return (ft_strlen(new->value));
}

int add_redir(t_token **tokens, char chr, char nextchr)
{
    if (chr == '|')
        return (add_token(tokens, "|", T_PIPE, NONE));
    else if (chr == '<' && nextchr == '<')
        return (add_token(tokens, "<<", T_HEREDOC, NONE));
    else if (chr == '<')
        return (add_token(tokens, "<", T_IN_REDIR, NONE));
    else if (chr == '>' && nextchr == '>')
        return (add_token(tokens, ">>", T_APPEND, NONE));
    else 
        return (add_token(tokens, ">", T_OUT_REDIR, NONE));
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
	while (str[i] && !is_space(str[i]) && !is_redir(str[i]))
		i++;
	return (i);
}

static char	*get_word(t_token **tokens, char *str, int *i, char quote_chr)
{
	int		start;
	char	*word;

	start = *i;
	if (quote_chr)
	{
		*i = skip_quoted(str, *i, quote_chr);
		if (*i == -1)
			return (NULL);
		word = ft_substr(str, start, *i - start - 1);
	}
	else
	{
		*i = skip_unquoted(str, *i);
		word = ft_substr(str, start, *i - start);
	}
    if (!word)
        exit_program(tokens, NULL, ERR_MEM);
    return (word);
}

int	add_word(t_token **tokens, char *str)
{
	int				i;
	char			quote_chr;
	char			*word;
	t_quote_type	quote_type;

    i = 0;
    quote_chr = '\0';
	quote_type = get_quote_type(str[i]);
	if (quote_type != NONE)
		quote_chr = str[i++];
	word = get_word(tokens, str, &i, quote_chr);
	if (!word)
		return (-1);
	if (*word)
		add_token(tokens, word, T_WORD, quote_type);
	free(word);
	return (i);
}

void lexer(char *command_line, t_token **tokens, int *last_exit)
{
    int i;
    int wordlen;

    i = 0;
    wordlen = 0;
    while (command_line[i])
    {
        if (is_space(command_line[i]))
            i++;
        else if (is_redir(command_line[i]))
            i += add_redir(tokens, command_line[i], command_line[i + 1]);            
        else
        {
            wordlen = add_word(tokens, &command_line[i]);
            if (wordlen == -1)
            {
                token_error_handling(tokens, last_exit);
                return ;
            }
            i += wordlen;
        }
    }
}



