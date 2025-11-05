/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yocto <yocto@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 15:26:48 by yocto             #+#    #+#             */
/*   Updated: 2025/11/03 17:06:18 by yocto            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// the chdir builtin changes the current working directory when there is error returns -1   
//cd /path/to/directory && ls	this hsould work

#include "minishell.h" 

char *get_env_value(t_env *env, const char *key)
{
    while (env)
    {
        if (strcmp(env->key, key) == 0)
            return env->value;
        env = env->next;
    }
    return NULL;
}
void set_env_value(t_env **env, const char *key, const char *value)
{
    t_env *tmp = *env;

    while (tmp)
    {
        if (strcmp(tmp->key, key) == 0)
        {
            free(tmp->value);
            tmp->value = strdup(value);
            return;
        }
        tmp = tmp->next;
    }
    t_env *new = malloc(sizeof(t_env));
    new->key = strdup(key);
    new->value = strdup(value);
    new->next = *env;
    *env = new;
}
void cd_builtin(t_data *data, t_arg *args)
{
    char *target_dir;
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
        set_env_value(&data->env, "OLDPWD", cwd);
    if (args == NULL || args->value == NULL)
    {
        target_dir = get_env_value(data->env, "HOME");
        if (target_dir == NULL)
        {
            write(STDERR_FILENO, "cd: HOME not set\n", 17);
            return;
        }
    }
    if (args && strcmp(args->value, "-") == 0)
    {
        target_dir = get_env_value(data->env, "OLDPWD");
        if (!target_dir)
        {
            write(STDERR_FILENO, "cd: OLDPWD not set\n", 19);
            return;
        }
        printf("%s\n", target_dir);
    }
    else
        target_dir = args->value;


    if (chdir(target_dir) != 0)
    {
        perror("cd");
        return;
    }
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        set_env_value(&data->env, "PWD", cwd);

}
