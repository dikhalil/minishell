/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_handle_format.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 21:45:56 by dikhalil          #+#    #+#             */
/*   Updated: 2025/08/24 23:26:13 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_handle_format(va_list args, const char format)
{
	int	count;

	count = 0;
	if (format == 'c')
		count += ft_putchr_o(va_arg(args, int));
	else if (format == 's')
		count += ft_putstr_o(va_arg(args, char *));
	else if (format == 'd' || format == 'u' || format == 'i')
	{
		if (format == 'd' || format == 'i')
			count += ft_putnbr_o((long)va_arg(args, int));
		else
			count += ft_putnbr_o(va_arg(args, unsigned long));
	}
	else if (format == 'x' || format == 'X')
		count += ft_puthex(va_arg(args, unsigned int), format);
	else if (format == 'p')
		count += ft_putptr(va_arg(args, void *));
	else
		count += ft_putchr_o(format);
	return (count);
}
