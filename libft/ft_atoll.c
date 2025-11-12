/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoll.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:17:33 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/12 09:15:26 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

long long ft_atoll(const char *str)
{
    long long result = 0;
    int sign = 1;
    int i = 0;

    if (str[i] == '-' || str[i] == '+')
        sign = (str[i++] == '-') ? -1 : 1;

    while (ft_isdigit(str[i]))
    {
        int digit = str[i] - '0';
        if (sign == 1 && (result > (LLONG_MAX - digit) / 10))
            return (LLONG_MAX); 
        if (sign == -1 && (-result < (LLONG_MIN + digit) / 10))
            return (LLONG_MIN); 
        result = result * 10 + digit;
        i++;
    }
    return (result * sign);
}
