/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putptr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 23:01:31 by dikhalil          #+#    #+#             */
/*   Updated: 2025/09/10 12:00:33 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putptr(void *ptr)
{
	int	count;

	count = 0;
	if (!ptr)
		count += ft_putstr_o("(nil)");
	else
	{
		count += ft_putstr_o("0x");
		count += ft_puthex((unsigned long)ptr, 'x');
	}
	return (count);
}
