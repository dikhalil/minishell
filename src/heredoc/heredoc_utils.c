/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 17:47:46 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/21 13:21:24 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void	heredoc_ctrl_d(char *delim, int i)
{
	write(1, "\n", 1);
	ft_putstr_fd("minishell: warning: here-document at line ", 2);
	ft_putnbr_fd(i, 2);
	ft_putstr_fd(" delimited by end-of-file (wanted `", 2);
	ft_putstr_fd(delim, 2);
	ft_putendl_fd("')", 2);
}

static int	is_lastheredoc(t_redir *redir)
{
	t_redir	*tmp;

	tmp = redir->next;
	while (tmp)
	{
		if (tmp->type == T_HEREDOC)
			return (0);
		tmp = tmp->next;
	}
	return (1);
}

static void	set_infile(t_cmd *cmd, t_redir *redir, int in_fd, int out_fd)
{
	if (g_sig == SIGINT)
	{
		close(in_fd);
		close(out_fd);
		cmd->infile = -1;
		return ;
	}
	if (is_lastheredoc(redir))
		cmd->infile = in_fd;
	else
		close(in_fd);
	close(out_fd);
}

void	handle_heredoc(t_data *data, t_cmd *cmd, t_redir *redir)
{
	int		end[2];
	char	*line;
	int		i;

	i = 0;
	if (pipe(end) == -1)
		exit_program(data, ERR_MEM);
	while (TRUE)
	{
		i++;
		line = get_next_line_stdin();
		if (g_sig == SIGINT)
		{
			data->last_exit = 130;
			break ;
		}
		if (!line || !ft_strcmp(line, redir->delim))
			break ;
		if (redir->quote == NONE && ft_strchr(line, '$'))
			expand_str(data, &line);
		ft_putendl_fd(line, end[1]);
		free(line);
	}
	if (!line && g_sig != SIGINT)
		heredoc_ctrl_d(redir->delim, i);
	if (line)
		free(line);
	set_infile(cmd, redir, end[0], end[1]);
}
