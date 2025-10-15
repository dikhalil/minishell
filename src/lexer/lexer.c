/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 15:13:07 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/15 18:19:53 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static int add_token(t_token **tokens, char *value, t_token_type type, t_quote_type quote)
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

static int add_redir(t_token **tokens, char chr, char nextchr)
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

static int	add_word(t_token **tokens, char *str)
{
	int				i;
	char			*word;
	t_quote_type	quote_type;

    i = 0;
    quote_type = NONE;
	word = get_word(tokens, str, &i, &quote_type);
	if (!word)
		return (-1);
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



