// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   executor.c                                         :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2025/10/18 21:07:48 by dikhalil          #+#    #+#             */
// /*   Updated: 2025/10/18 21:10:26 by dikhalil         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// void executor(t_data *data)
// {
//     // inside the child
//     signal(SIGINT, SIG_DFL);
//     signal(SIGQUIT, SIG_DFL);

//     waitpid(pid, &status, 0);
//     if (WIFSIGNALED(status))
//     {
//         data->last_exit = 128 + WTERMSIG(status);
//     }

// }