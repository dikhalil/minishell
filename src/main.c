/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 14:41:49 by dikhalil          #+#    #+#             */
/*   Updated: 2025/10/22 21:10:40 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

#include <stdio.h>

//char *type(t_token_type type)
//{
//	if (type ==  T_PIPE)
//		return ("|");
//	if (type ==  T_IN_REDIR)
//		return ("<");
//	if (type ==   T_OUT_REDIR)
//		return (">");
//	if (type == T_APPEND)
//		return (">>");
//	if (type ==  T_HEREDOC)
//		return ("<<");
//	return ("");
//}
// void print_cmds_after_expand(t_data *data)
// {
//     t_cmd *cmd;
//     t_arg *arg;
//	 t_redir *redir;

//     cmd = data->cmds;
//     while (cmd)
//     {
//		printf("-----cmd-----\n");
//         arg = cmd->arg;
//         while (arg)
//         {
//             printf("ARG: %s\n", arg->value);
//             arg = arg->next;
//         }
//		 redir = cmd->redir;
//		 while (redir)
//		 {
//			printf("redir type: %s\n", type(redir->type));
//			if (redir->file)
//				printf("redir file: %s\n", redir->file);
//			else if (redir->delim)
//				printf("redir deilm: %s\n", redir->delim);
//             redir = redir->next;
//		 }
//         cmd = cmd->next;
//     }
// }

int main(int argc, char **argv, char **envp)
{
    t_data data;

    ft_memset(&data, 0, sizeof(t_data));
    init_env(&data, envp);
    data.argv = argv;
    data.argc = argc;
    while (TRUE)
    {
        data.command_line = readline(PROMPT);
        if (!data.command_line || !ft_strcmp(data.command_line, "exit"))
            exit_program(&data, 0);
        if (*data.command_line)
            add_history(data.command_line);
        else
        {
            free(data.command_line);
            continue;
        }
        data.tokens = NULL;
        lexer(&data);
        if (!data.tokens)
            continue ;
        data.cmds = NULL;
        parser(&data);
        if (!data.cmds)
            continue ;
        heredoc(&data);
        expand(&data);
		executor(&data, envp);
        token_clear(&data.tokens);
        cmd_clear(&data.cmds);
        free(data.command_line);
    }
    env_clear(&data.env);
    return (0);
}
