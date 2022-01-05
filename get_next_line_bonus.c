/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryoakira <ryoakira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/26 19:08:01 by blyu              #+#    #+#             */
/*   Updated: 2021/11/28 11:49:35 by ryoakira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"
#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif
#if BUFFER_SIZE <= 0
# undef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

typedef struct s_list
{
	int				fdi;
	size_t			read;
	struct t_list	*content;
	struct s_list	*next;
}	t_fd;

typedef struct t_list
{
	char			content[BUFFER_SIZE];
	ssize_t			len;
	struct t_list	*next;
}	t_unit;

enum
{
	T_FD,
	T_UNIT
};

enum
{
	ALL,
	SET,
};

static t_fd		*find_line(int fd);
static size_t	len_cpy(char *buf, size_t *start, t_unit *data, int fd);
static void		*free_line(void *l, t_fd *f, t_unit *u, int ctr);
static void		*new(int fd, int order);
void			thank(void);

char	*get_next_line(int fd)
{
	t_fd		*now;
	size_t		len;
	char		*s;

	if (fd < 0)
		return (free_line(NULL, find_line(-1), NULL, ALL));
	now = find_line(fd);
	if (!now)
		return (NULL);
	len = len_cpy(NULL, &(now->read), now->content, fd);
	if (!len)
		return (free_line(NULL, find_line(-1), NULL, ALL));
	s = malloc(len + 1);
	if (!s)
		return (free_line(NULL, find_line(-1), NULL, ALL));
	len_cpy(s, &(now->read), now->content, fd);
	free_line(now, find_line(-1), NULL, SET);
	s[len] = '\0';
	return (s);
	thank();
}

static t_fd	*find_line(int	fd)
{
	static t_fd	*start = NULL;
	t_fd		*l;

	if (fd == -33 && !(start->next))
	{
		free(start->content);
		free(start);
		start = NULL;
	}
	if (fd == -42)
		start = NULL;
	if (fd < 0)
		return (start);
	if (!start)
		start = new(-1, T_FD);
	l = start;
	while (l && l->next && l->fdi != fd)
		l = l->next;
	if (l && !(l->next) && l->fdi != fd)
	{
		l->next = new(fd, T_FD);
		l = l->next;
	}
	return (l);
}

static void	*new(int fd, int order)
{
	t_fd	*new_fd;
	t_unit	*new_unit;

	new_unit = malloc(sizeof(t_unit));
	if (!new_unit)
		return (free_line(NULL, find_line(-1), NULL, ALL));
	new_unit->next = NULL;
	if (fd >= 0)
		new_unit->len = read(fd, new_unit->content, BUFFER_SIZE);
	if (fd >= 0 && new_unit->len < 0)
		return (free_line(new_unit, find_line(-1), NULL, ALL));
	if (order == T_UNIT)
		return (new_unit);
	new_fd = malloc(sizeof(t_fd));
	if (!new_fd)
		return (free_line(new_unit, find_line(-1), NULL, ALL));
	new_fd->fdi = fd;
	new_fd->read = 0;
	new_fd->content = new_unit;
	new_fd->next = NULL;
	return (new_fd);
}

static size_t	len_cpy(char *buf, size_t *start, t_unit *data, int fd)
{
	size_t	i;
	char	c;

	i = *start;
	c = (data->content)[i % BUFFER_SIZE];
	while (data && ((c != '\n' && c != '\0') || !(i - *start)) && \
	i % BUFFER_SIZE < (unsigned long)(data->len))
	{
		c = (data->content)[i % BUFFER_SIZE];
		if (buf)
			buf[i - *start] = c;
		i++;
		if (!(i % BUFFER_SIZE))
		{
			if (!data->next)
				data->next = new(fd, T_UNIT);
			data = data->next;
			if (!data)
				return (0);
		}
	}
	if (buf)
		*start = i % BUFFER_SIZE;
	return (i - *start);
}

static void	*free_line(void *l, t_fd *f, t_unit *u, int ctr)
{
	t_fd	*fb;
	t_unit	*ub;

	while (f)
	{
		u = f->content;
		while (u && (ctr == ALL || (f == l && \
		(u->next || (size_t)u->len == f->read))))
		{
			ub = u;
			u = u->next;
			free (ub);
		}
		f->content = u;
		if (f == l && !u && ctr == SET)
			fb->next = f->next;
		fb = f;
		f = f->next;
		if ((fb == l && !u) || ctr == ALL)
			free (fb);
		if (fb == l && ctr == SET)
			return (find_line(-33));
	}
	free (l);
	return (find_line(-42));
}

/*

#include <stdio.h>
#include <fcntl.h>
int main (void)
{
	char *s;
	char *s1;
	char *s2;
	char *s3;
	int fd;
	int fd1;
	int fd2;
	int fd3;
	int i;

	(void)fd;
	(void)s;
	(void)s1;
	(void)s2;
	(void)s3;
	(void)fd;
	(void)fd1;
	(void)fd2;
	(void)fd3;

	//A file
	printf ("\n\nA file\n");
	fd = open("./files/news.txt", O_RDONLY);
	i = 0;
	while (1)
	{
		s = get_next_line(fd);
		if (!s)
		{
			printf ("(NULL)\n");
			break ;
		}
		printf ("[%d]%s", i, s);
		free(s);
		i++;
	}
	close(fd);

	//Standeard input
	printf ("\n\nStandeard input\n");
	s = get_next_line(0);
	printf("s is... [\\n]\n%s[\\n]\n", s);
	free(s);
	close(0);

	//Three files
	printf ("\n\nThree files\n");
	fd1 = open("./files/ppap1.txt", O_RDONLY);
	fd2 = open("./files/ppap2.txt", O_RDONLY);
	fd3 = open("./files/ppap3.txt", O_RDONLY);
	while (1)
	{
		//ppap1.txt
		s1 = get_next_line(fd1);
		if (!s1)
		{
			printf ("(NULL)\n");
			break ;
		}
		printf ("%s",s1);
		free(s1);

		//ppap2.txt
		s2 = get_next_line(fd2);
		if (!s2)
		{
			printf ("(NULL)\n");
			break ;
		}
		printf ("%s",s2);
		free(s2);

		//ppap3.txt
		s3 = get_next_line(fd3);
		if (!s3)
		{
			printf ("(NULL)\n");
			break ;
		}
		printf ("%s",s3);
		free(s3);
	}
	close(fd1);
	close(fd2);
	close(fd3);
	return(0);
}
*/
