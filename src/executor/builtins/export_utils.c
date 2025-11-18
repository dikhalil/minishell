/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 18:35:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/11/18 18:36:00 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*strdup_until_char(const char *str, char c)
{
	size_t	len;
	char	*dup;
	size_t	i;

	len = 0;
	while (str[len] && str[len] != c)
		len++;
	dup = malloc(len + 1);
	if (!dup)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dup[i] = str[i];
		i++;
	}
	dup[len] = '\0';
	return (dup);
}

int	valid_identifier(const char *str)
{
	int	i;

	if (!str || (!ft_isalpha(str[0]) && str[0] != '_'))
		return (0);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

void	swap_env_nodes(t_env *current, t_env *next)
{
	char	*temp_key;
	char	*temp_value;

	temp_key = current->key;
	temp_value = current->value;
	current->key = next->key;
	current->value = next->value;
	next->key = temp_key;
	next->value = temp_value;
}

void	sort_env_list(t_env *env)
{
	t_env	*current;
	t_env	*next;
	int		swapped;

	swapped = 1;
	while (swapped)
	{
		swapped = 0;
		current = env;
		while (current->next)
		{
			next = current->next;
			if (ft_strcmp(current->key, next->key) > 0)
			{
				swap_env_nodes(current, next);
				swapped = 1;
			}
			current = current->next;
		}
	}
}

void	print_env_entry(t_env *current)
{
	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	ft_putstr_fd(current->key, STDOUT_FILENO);
	if (current->value)
	{
		ft_putstr_fd("=\"", STDOUT_FILENO);
		ft_putstr_fd(current->value, STDOUT_FILENO);
		ft_putstr_fd("\"", STDOUT_FILENO);
	}
	ft_putchar_fd('\n', STDOUT_FILENO);
}
