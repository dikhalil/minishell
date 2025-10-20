/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 23:00:51 by dikhalil          #+#    #+#             */
/*   Updated: 2025/08/24 23:26:22 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_puthex(unsigned long n, char format)
{
	int		count;
	char	*hexa;

	count = 0;
	if (format == 'x')
		hexa = "0123456789abcdef";
	else
		hexa = "0123456789ABCDEF";
	if (n >= 16)
		count += ft_puthex(n / 16, format);
	count += ft_putchr_o(hexa[n % 16]);
	return (count);
}
