/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 19:42:32 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/20 22:45:19 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void prompt_signals_handler(int sig)
{
    g_sig = sig;
    if (g_sig == SIGINT)
        write(1, "\n", 1);
    rl_replace_line("", 0);
    rl_on_new_line();
    rl_redisplay();
}

static void heredoc_signals_handler(int sig)
{
    g_sig = sig;
    write(1, "\n", 1);
}

void set_prompt_signal(void)
{
    signal(SIGINT, prompt_signals_handler);
    signal(SIGQUIT, prompt_signals_handler);
}

void set_heredoc_signal(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;

    ft_memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = heredoc_signals_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);
    ft_memset(&sa_quit, 0, sizeof(sa_quit));
    sa_quit.sa_handler = SIG_IGN;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);
}
