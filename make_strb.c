#include "minishell.h"


int ft_tablen(char **tab)
{
	int i;

	i = 0;
	while (tab[i])
		i++;
	return (i);
}

int num_of_redirect(char **tab)
{
	int i;
	int res;

	i = 0;
	res = 0;
	while (tab[i])
	{
		if (ft_strcmp(tab[i], ">") == 0 || ft_strcmp(tab[i], ">>") == 0 || ft_strcmp(tab[i], "<") == 0)
			res++;
		i++;
	}
	return (res);
}


char *separate_redirect(char *command);


int is_space(char c)
{
	if (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
		return (1);
	return (0);
}

int is_redirect(char c)
{
	if (c == '<' || c == '>')
		return (1);
	return (0);
}

int is_ok(char *command, int i)
{
	int len = ft_strlen(command);

	if (command[i] == '<')
		return (1);
	if (i && command[i - 1] == '<')
		return (1);
	if (i < len - 1 && command[i] == '>' && command[i + 1] == '>')
		return (1);
	if (1 < i && command[i] != '>' && command[i - 1] == '>' && command[i - 2] == '>')
		return (1);
	if (0 < i && i < len - 1 && command[i] == '>' && command[i - 1] != '>' && command[i + 1] != '>')
		return (1);
	if (0 < i && command[i] != '>' && command[i - 1] == '>' && command[i - 2])
		return (1);
	return (0);
}

char *set_res(char *res, char *command)
{
	int i;
	int j;
	char flag;
	char flag2;

	flag = 0;
	flag2 = 0;
	i = 0;
	j = 0;
	while (command[i])
	{
		check_quote(command[i], &flag);
		if ((!flag || flag2 < flag) && is_ok(command, i))
			res[j++] = ' ';
		res[j++] = command[i++];
		flag2 = flag;
	}
	res[j] = '\0';
	return (res);
}

char *separate_redirect(char *command)
{
	int i;
	int redirect;
	char *res;

	i = 0;
	redirect = 0;
	while (command[i])
	{
		if (is_redirect(command[i]))
			redirect++;
		i++;
	}
	res = malloc(sizeof(char) * (i + redirect * 2 + 1));
	return (set_res(res, command));
}

char **really_make_strb(char **str_a, int *table)
{
	int i;
	int j;
	int len;
	int cnt_redirect;
	char **str_b;
	
	i = 0;
	j = 0;
	len = ft_tablen(str_a);
	cnt_redirect = num_of_redirect(str_a);
	str_b = malloc(sizeof(char *) * (len - cnt_redirect * 2 + 1));
	while (str_a[i])
	{
		if (table[i] == 1)
		{
			i++;
			continue;
		}
		str_b[j++] = str_a[i++];
	}
	str_b[j] = NULL;
	return (str_b);
}

char **make_strb(char **str_a)
{
	int i;
	int *table;

	i = 0;
	table = malloc(sizeof(int) * ft_tablen(str_a));
	while (str_a[i])
	{
		if (ft_strcmp(str_a[i], ">>") == 0 || ft_strcmp(str_a[i], ">") == 0)
		{
			table[i] = 1;
			table[i + 1] = 1;
			i++;
		}
		else if (ft_strcmp(str_a[i], "<") == 0)
		{
			table[i] = 1;
			table[i + 1] = 1;
			i++;
		}
		else
			table[i] = 0;
		i++;
	}
	return (really_make_strb(str_a, table));
}