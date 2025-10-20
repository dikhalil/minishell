/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 16:00:13 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/20 21:14:07 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static size_t	safe_strlen(char *s)
{
	size_t	len = 0;
	if (!s)
		return (0);
	while (s[len])
		len++;
	return (len);
}

char	*str_join_free(char *s1, char *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	str = malloc(safe_strlen(s1) + safe_strlen(s2) + 1);
	if (!str)
	{
		free(s1);
		free(s2);
		return (NULL);
	}
	while (s1 && s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	while (s2 && s2[j])
		str[i++] = s2[j++];
	str[i] = '\0';
	free(s1);
	free(s2);
	return (str);
}

int	is_number(char *str)
{
	int	j;

	j = 0;
	if (str[0] == '-' || str[0] == '+')
		j++;
	while (str[j])
	{
		if (!ft_isdigit(str[j]))
			return (0);
		j++;
	}
	return (1);
}

void	free_split(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

char *str_join_chr(char *s, char c)
{
    char	*str;
	size_t	len;
	int		i;

	i = 0;
    len = 0;
    if (s)
       len = ft_strlen(s);
	str = malloc(len + 2);
	if (str)
	{
        while (s && s[i])
        {
            str[i] = s[i];
            i++;
        }   
        str[i++] = c;
        str[i] = 0;
    }
    free(s);
	return (str);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	unsigned char	*s_1;
	unsigned char	*s_2;

	s_1 = (unsigned char *)s1;
	s_2 = (unsigned char *)s2;
	while (*s_1 && *s_1 == *s_2)
	{
		s_1++;
		s_2++;
	}
	return (*s_1 - *s_2);
}
