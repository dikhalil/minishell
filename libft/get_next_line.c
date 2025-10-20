/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 18:59:39 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/20 21:45:33 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"


static int	handle_read(char *buf, ssize_t bytes, char **buffer)
{
	if (bytes < 0)
	{
		free(*buffer);
		*buffer = NULL;
		return (0);
	}
	if (bytes == 0)
	{
		if ((*buffer)[0] == '\0')
		{
			free(*buffer);
			*buffer = NULL;
			return (0);
		}
		return (1);
	}
	buf[bytes] = '\0';
	if (buf[0] == '\n')
		return (2);
	*buffer = ft_strjoin_gnl(*buffer, buf);
	if (!*buffer)
		return (0);
	return (3);
}

static char	*read_until_newline(char *buf)
{
	char	*buffer;
	ssize_t	bytes;
	int		status;

	buffer = ft_strdup_gnl("");
	if (!buffer)
		return (NULL);
	while (1)
	{
		bytes = read(0, buf, 1);
		status = handle_read(buf, bytes, &buffer);
		if (status == 0)
			return (NULL);
		if (status == 1 || status == 2)
			break ;
	}
	return (buffer);
}

char	*get_next_line_stdin(void)
{
	char	*buf;
	char	*buffer;

	write(1, "> ", 2);
	buf = malloc(2);
	if (!buf)
		return (NULL);
	buffer = read_until_newline(buf);
	free(buf);
	return (buffer);
}