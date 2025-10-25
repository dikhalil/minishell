/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_o.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 22:59:47 by dikhalil          #+#    #+#             */
/*   Updated: 2025/08/24 23:26:28 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putstr_o(char *s)
{
	if (!s)
	{
		ft_putstr_fd("(null)", 1);
		return (ft_strlen("(null)"));
	}
	ft_putstr_fd(s, 1);
	return (ft_strlen(s));
}
