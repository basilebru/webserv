/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrunet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/25 10:01:26 by bbrunet           #+#    #+#             */
/*   Updated: 2019/12/19 14:15:26 by bbrunet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int		ft_sub(char **dst, char *src, int start, int len)
{
	int i;

	if (!(*dst = (char*)malloc((len + 1) * sizeof(**dst))))
		return (-1);
	i = 0;
	while (i < len)
	{
		dst[0][i] = src[start + i];
		i++;
	}
	dst[0][i] = 0;
	return (0);
}

int		ft_cut(char **str, char **line)
{
	int		i;
	char	*tmp;

	i = 0;
	while (str[0][i] != '\n')
		i++;
	if (ft_sub(line, *str, 0, i) == -1)
	{
		free(*str);
		*str = 0;
		return (-1);
	}
	tmp = *str;
	if (ft_sub(str, tmp, i + 1, ft_len(*str) - i - 1) == -1)
	{
		free(tmp);
		tmp = 0;
		return (-1);
	}
	free(tmp);
	return (0);
}

int		get_next_line(int fd, char **line)
{
	static char	*str;
	char		*buf;
	int			ret;
	int			cat;

	ret = 0;
	cat = 0;
	while (fd >= 0 && line && !f_line(str) && (ret = ft_read(&buf, fd)) &&
ret != -1 && (cat = ft_cat(&str, buf, ret)) != -1)
		fd = fd + 0;
	if (ret == -1 || cat == -1 || fd < 0 || !line)
		return (ft_clean(-1, &str, 0));
	if (str == 0 || (!ret && !(f_line(str))))
	{
		cat = ft_sub(line, str, 0, ft_len(str));
		return (ft_clean(0, &str, cat));
	}
	if (ft_cut(&str, line) == -1)
		return (ft_clean(-1, &str, 0));
	return (1);
}
