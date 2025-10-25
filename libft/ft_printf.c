/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 21:29:19 by dikhalil          #+#    #+#             */
/*   Updated: 2025/08/24 23:26:17 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int		count;

	count = 0;
	if (!format)
		return (-1);
	va_start(args, format);
	while (*format)
	{
		if (*format == '%')
		{
			format++;
			count += ft_handle_format(args, *format);
		}
		else
			count += ft_putchr_o(*format);
		format++;
	}
	va_end(args);
	return (count);
}
