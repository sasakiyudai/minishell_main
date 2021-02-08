/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/06 18:00:21 by syudai            #+#    #+#             */
/*   Updated: 2021/02/08 16:29:13 by rnitta           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <errno.h>
#include <string.h>
#define MAX_FILENAME 1024

void	cd_error(char **args)
{
	ft_putstr_fd("cd: ", 2);
	if (args[1])
	{
		ft_putstr_fd(args[1], 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
	}
	else
	{
		ft_putstr_fd("", 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd("No such file or directory", 2);
	}
}

int		update(t_arg_main *arg_main)
{
	char	cwd[MAX_FILENAME];
	t_arg	arg;

	if (getcwd(cwd, MAX_FILENAME) == NULL)
		return (1);
	arg.data = cwd;
	arg.name = "OLDPWD";
	arg.type = ARG_TYPE_STR;
	arg_add(arg_main, &arg);
	return (0);
}

void	update_pwd(t_arg_main *arg_main)
{
	char	s[MAX_FILENAME];
	t_arg	arg;

	if (!getcwd(s, MAX_FILENAME))
		return ;
	arg.data = s;
	arg.name = "PWD";
	arg.type = ARG_TYPE_STR;
	arg_add(arg_main, &arg);
}

int		ft_cd(char **args, t_arg_main *arg_main)
{
	int		cd_ret;
	char	*dest;
	t_arg	arg;

	if (ft_len(args) == 1)
	{
		if (arg_get(arg_main, &arg, "HOME") || arg.data == NULL)
			return (1 + 0 * (write(2, "bash: cd: HOME not set", 22)));
		dest = ft_strdup(arg.data);
		arg_free(&arg);
	}
	else
		dest = ft_strdup(args[1]);
	update(arg_main);
	if (!dest[0])
	{
		free(dest);
		return (0);
	}
	cd_ret = chdir(dest);
	if (cd_ret < 0)
		cd_ret *= -1;
	if (cd_ret != 0)
		cd_error(args);
	if (cd_ret == 0)
	{
		update_pwd(arg_main);
		if (ft_strlen(dest) >= 1 && dest[0] == '/' && dest[1] != '/')
			arg_main->pwd_slash = 0;
		if (ft_strlen(dest) >= 2	&&
			dest[0] == '/' && dest[1] == '/')
			arg_main->pwd_slash = dest[2] != '/';
	}
	free(dest);
	return (cd_ret);
}
