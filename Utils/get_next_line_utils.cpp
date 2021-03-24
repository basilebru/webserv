/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrunet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/25 15:12:01 by bbrunet           #+#    #+#             */
/*   Updated: 2019/12/19 14:15:25 by bbrunet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int		f_line(char *str)
{
	int i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] == '\n')
			return (1);
		i++;
	}
	return (0);
}

int		ft_read(char **buf, int fd)
{
	int ret;

	if (BUFFER_SIZE == 0)
		return (-1);
	if (!(*buf = (char *)malloc((BUFFER_SIZE + 1) * sizeof(*buf))))
		return (-1);
	ret = read(fd, *buf, BUFFER_SIZE);
	if (ret == -1)
		return (-1);
	buf[0][ret] = 0;
	if (!ret)
	{
		free(*buf);
		*buf = 0;
	}
	return (ret);
}

int		ft_len(char *str)
{
	int i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

int		ft_cat(char **str, char *buf, int ret)
{
	char	*tmp;
	int		i;
	int		len;

	tmp = *str;
	len = ft_len(*str);
	if (!(*str = (char *)malloc(len + ret + 1)))
	{
		if (tmp)
			free(tmp);
		free(buf);
		return (-1);
	}
	i = -1;
	while (++i < len)
		str[0][i] = tmp[i];
	len = -1;
	while (buf[++len])
		str[0][i + len] = buf[len];
	str[0][i + len] = 0;
	if (tmp)
		free(tmp);
	free(buf);
	return (0);
}

int		ft_clean(int ret, char **str, int error)
{
	if (*str)
	{
		free(*str);
		*str = 0;
	}
	if (error == -1)
		return (-1);
	return (ret);
}
