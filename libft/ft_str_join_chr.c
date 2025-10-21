/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_str_join_chr.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 14:06:17 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/21 14:06:28 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

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
