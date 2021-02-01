/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: syudai <syudai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/28 17:47:28 by syudai            #+#    #+#             */
/*   Updated: 2021/02/02 01:18:17 by syudai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	set_fd(char ***cmd, char ***raw_cmd, int *fd, int j)
{
	if (*(cmd + 1) != NULL)
		dup2(fd[j + 1], 1);
	if (j != 0)
		dup2(fd[j - 2], 0);
	set_right(raw_cmd, j, fd, 1);
	set_left(raw_cmd, j, fd, 1);
}

void	wait_chiledren_and_free_fd(int cmd_len, int *fd, pid_t *pids, t_arg_main *arg_main)
{
	int i;
	int status;

	i = 0;
	//while (i < 2 * cmd_len)
	//	close(fd[i++]);
	i = 0;
	/* while (i < cmd_len)
	{
		// wait(NULL);
		waitpid(pids[i], &status, 0);
		i++;
	} */

	// int cnt = cmd_len;

    /* while (cnt)
    {
        i = 0;
        while (i < cmd_len)
        {
            waitpid(pids[i], &status, WNOHANG);
            if (WIFEXITED(status))
            {
				printf("%d\n", i);
                close(fd[i]);
                close(fd[i + 1]);
                cnt--;
                if (i == cmd_len - 1)
                    set_hatena(g_arg_main, WEXITSTATUS(status));
            }
            i++;
        }
    } */

	pid_t ret;
    int cnt = cmd_len;
    int j;
	char c = 4;
    while (cnt--)
    {
        ret = waitpid(-1, &status, 0);
        j = -1;
		printf("68にきた\n");
        while (++j < cmd_len)
        {
            if (ret == pids[j])
            {
                if (j != 0)
                    printf("%d\n", close(fd[j * 2 - 2]));
                if (j != cmd_len - 1)
				{
					write(fd[j * 2 + 1], &c, 1);
                    printf("%d\n", close(fd[j * 2 + 1]));
				}
                else
                    set_hatena(g_arg_main, WEXITSTATUS(status));
            }
        }
    }
	
	free(pids);
	free(fd);
	set_hatena(arg_main, WEXITSTATUS(status)); //　反映されない？？？
}

int		error(char *path)
{
	DIR	*folder;
	int	fd;
	int	exit_code;
	
	fd = open(path, O_WRONLY);
	folder = opendir(path);
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(path, 2);
	if (ft_strchr(path, '/') == NULL)
		ft_putendl_fd(": command not found", 2);
	else if (fd == -1 && folder == NULL)
		ft_putendl_fd(": No such file or directory", 2);
	else if (fd == -1 && folder != NULL)
		ft_putendl_fd(": is a directory", 2);
	else if (fd != -1 && folder == NULL)
		ft_putendl_fd(": Permission denied", 2);
	if (ft_strchr(path, '/') == NULL || (fd == -1 && folder == NULL))
		exit_code = 127;
	else
		exit_code = 126;
	if (folder)
		closedir(folder);
	safe_close(fd);
	return (exit_code);
}

int		is_builtin(char *command)
{
	if (ft_strcmp(command, "echo") == 0)
		return (1);
	else if (ft_strcmp(command, "cd") == 0)
		return (2);
	else if (ft_strcmp(command, "pwd") == 0)
		return (3);
	else if (ft_strcmp(command, "export") == 0)
		return (4);
	else if (ft_strcmp(command, "unset") == 0)
		return (5);
	else if (ft_strcmp(command, "env") == 0)
		return (6);
	else if (ft_strcmp(command, "exit") == 0)
		return (7);
	return (0);
}

void	call_builtin(int tmp, char **str_b, t_arg_main *arg_main, char **envs)
{
	int result;

	result = 0;
	if (tmp == 1)
		ft_echo(str_b);
	else if (tmp == 2)
		result = ft_cd(str_b, arg_main);
	else if (tmp == 3)
		result = ft_pwd();
	else if (tmp == 4)
		ft_export(str_b, envs, arg_main);
	else if (tmp == 5)
		ft_unset(str_b, arg_main);
	else if (tmp == 6)
		ft_env(envs);
	else if (tmp == 7)
	{
		write(2, "exit\n", 5);
		exit(0);
	}
	set_hatena(arg_main, result);
}

void print_tab(char *env[])
{
	int i = 0;

	while (env[i])
		fprintf(stdout, "%s\n", env[i++]);
}

void	exec_child(int cmd_len, int *fd, char ***cmd, t_arg_main *arg_main)
{
	int		exit_code;
	int		i;
	int		tmp;
	char	*path;
	char	**envs;

	i = 0;
	if (!(envs = arg_list_get(arg_main)))
		exit(1); //　これでいいのか
	while (i < 2 * cmd_len)
		close(fd[i++]);

	if ((tmp = is_builtin((*cmd)[0])))
	{
		call_builtin(tmp, *cmd, arg_main, envs);
		//close(0);
		//close(1);
		exit(0);
	}
	else
	{
		if (0 == (tmp = get_path(arg_main, &path, (*cmd)[0])))
		{
			execve(path, *cmd, envs);
		}
		else if (tmp == -1)
			print_error(MALLOC_FAIL);
		exit_code = error((*cmd)[0]);
		exit(exit_code);
	}
}

void	pipeline2(char ***cmd, char ***raw_cmd, t_arg_main *arg_main)
{
	int		i;
	int		j;
	pid_t	pid;
	pid_t	*pids;
	int		*fd;

	if (!(fd = malloc(sizeof(int) * 2 * count(cmd))) || !(pids = malloc(sizeof(pid_t) * count(cmd))))
		return (print_error(MALLOC_FAIL));
	i = 0;
	j = 0;
	while (i < count(cmd))
		pipe(fd + i++ * 2);
	while (*cmd != NULL)
	{
		pid = fork();
		if (pid == 0)
		{
			set_fd(cmd, raw_cmd, fd, j);
			exec_child(count(cmd), fd, cmd, arg_main);
		}
		pids[j / 2] = pid;
		cmd++;
		j += 2;
	}
	wait_chiledren_and_free_fd(i, fd, pids, arg_main);
}

void	pipeline(char ***cmd, char ***raw_cmd, t_arg_main *arg_main)
{
	
	if (count(cmd) == 1)
		one_command(cmd, raw_cmd, arg_main);
	else if (count(cmd) >= 1)
		pipeline2(cmd, raw_cmd, arg_main);
}
