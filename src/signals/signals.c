/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 19:42:32 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/21 14:24:57 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

static void main_handler(int sig)
{
    g_sig = sig;
    if (g_sig == SIGINT)
        write(1, "\n", 1);
    rl_replace_line("", 0);
    rl_on_new_line();
    rl_redisplay();
}

static void heredoc_handler(int sig)
{
    g_sig = sig;
    write(1, "\n", 1);
}

void set_main_signal(void)
{
    struct sigaction sa;

    ft_memset(&sa, 0, sizeof(sa));
    sa.sa_handler = main_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

void set_heredoc_signal(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;

    ft_memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = heredoc_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);
    ft_memset(&sa_quit, 0, sizeof(sa_quit));
    sa_quit.sa_handler = SIG_IGN;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);
}

void set_child_signal(void)
{
    struct sigaction sa;

    ft_memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}
