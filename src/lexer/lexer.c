/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 15:13:07 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/13 20:13:00 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

int add_token(t_token **head, char *value, t_token_type type, t_quote_type quote)
{
    t_token *new;

    new = malloc(sizeof(t_token));
    if (!new)
        exit_program(head, ERR_MEM);
    new->value = ft_strdup(value);
    new->quote = quote;
    new->type = type;
    new->next = NULL;
    token_add_back(head, new);
    return (ft_strlen(new->value));
}

int add_redir(t_token **token, char chr, char nextchr)
{
    if (chr == '|')
        return (add_token(token, "|", T_PIPE, NONE));
    else if (chr == '<' && nextchr == '<')
        return (add_token(token, "<<", T_HEREDOC, NONE));
    else if (chr == '<')
        return (add_token(token, "<", T_IN_REDIR, NONE));
    else if (chr == '>' && nextchr == '>')
        return (add_token(token, ">>", T_APPEND, NONE));
    else 
        return (add_token(token, ">", T_OUT_REDIR, NONE));
}

int add_word(t_token **head, char *str)
{
    int i;
    int start;
    char quote_chr;
    char *word;
    t_quote_type quote_type;

    i = 0;
    quote_chr = '\0';
    quote_type = get_quote_type(str[i]);
    if (quote_type != NONE)
        quote_chr = str[i++];
    start = i;
    if (quote_chr)
    {
        while (str[i] && str[i] != quote_chr)
            i++;
        if (str[i] != quote_chr)
            return (-1);
        i++;
        word = ft_substr(str, start, i - start - 1);
    }
    else
    {
        while (str[i] && !is_space(str[i]) && !is_redir(str[i]))
            i++;
        word = ft_substr(str, start, i - start);
    }
    if (!word)
        exit_program(head, ERR_MEM);
    if (*word)
        add_token(head, word, T_WORD, quote_type);
    free(word);
    return (i);
}

void lexer(char *command_line, t_token **token, int *last_exit)
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
            i += add_redir(token, command_line[i], command_line[i + 1]);            
        else
        {
            wordlen = add_word(token, &command_line[i]);
            if (wordlen == -1)
            {
                error_handling(token, last_exit, "syntax error: unclosed quotes", ERR_QUOTE);
                return ;
            }
            i += wordlen;
        }
    }
}



