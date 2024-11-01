
#include "bircd.h"

void	check_fd(t_env *e)
{
	int	i;

	i = 0;
	while ((i < e->maxfd) && (e->r > 0))
	{
		if (FD_ISSET(i, &e->fd_read))
			e->fds[i].fct_read(e, i);
		if (FD_ISSET(i, &e->fd_write))
			e->fds[i].fct_write(e, i);
		if (FD_ISSET(i, &e->fd_read) || FD_ISSET(i, &e->fd_write))
			e->r--;
		i++;
	}
}

#include "bircd.h"
#include <stdlib.h>

void	clean_fd(t_fd *fd)
{
	fd->type = FD_FREE;
	fd->fct_read = NULL;
	fd->fct_write = NULL;
}

#include "bircd.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

void	client_read(t_env *e, int cs)
{
	int	r;
	int	i;

	r = recv(cs, e->fds[cs].buf_read, BUF_SIZE, 0);
	if (r <= 0)
	{
		close(cs);
		clean_fd(&e->fds[cs]);
		printf("client #%d gone away\n", cs);
	}
	else
	{
		i = 0;
		while (i < e->maxfd)
		{
			if ((e->fds[i].type == FD_CLIENT) && (i != cs))
				send(i, e->fds[cs].buf_read, r, 0);
			i++;
		}
	}
}

#include "bircd.h"
#include <string.h>
#include <sys/socket.h>

void	client_write(t_env *e, int cs)
{
	int	len;

	len = strlen(e->fds[cs].buf_write);
	if (len > 0)
	{
		send(cs, e->fds[cs].buf_write, len, 0);
		e->fds[cs].buf_write[0] = '\0';
	}
}

#include "bircd.h"
#include <stdlib.h>

void	do_select(t_env *e)
{
	e->r = select(e->max + 1, &e->fd_read, &e->fd_write, NULL, NULL);
}

#include "bircd.h"
#include <stdio.h>
#include <stdlib.h>

void	get_opt(t_env *e, int ac, char **av)
{
	if (ac != 2)
	{
		fprintf(stderr, USAGE, av[0]);
		exit(1);
	}
	e->port = atoi(av[1]);
}

#include "bircd.h"
#include <stdlib.h>
#include <sys/resource.h>

void	init_env(t_env *e)
{
	int				i;
	struct rlimit	rlp;

	X(-1, getrlimit(RLIMIT_NOFILE, &rlp), "getrlimit");
	e->maxfd = rlp.rlim_cur;
	e->fds = (t_fd *)Xv(NULL, malloc(sizeof(*e->fds) * e->maxfd), "malloc");
	i = 0;
	while (i < e->maxfd)
	{
		clean_fd(&e->fds[i]);
		i++;
	}
}

#include "bircd.h"
#include <string.h>
#include <sys/select.h>

void	init_fd(t_env *e)
{
	int	i;

	i = 0;
	e->max = 0;
	FD_ZERO(&e->fd_read);
	FD_ZERO(&e->fd_write);
	while (i < e->maxfd)
	{
		if (e->fds[i].type != FD_FREE)
		{
			FD_SET(i, &e->fd_read);
			if (strlen(e->fds[i].buf_write) > 0)
			{
				FD_SET(i, &e->fd_write);
			}
			e->max = MAX(e->max, i);
		}
		i++;
	}
}

#include "bircd.h"

int	main(int ac, char **av)
{
	t_env	e;

	init_env(&e);
	get_opt(&e, ac, av);
	srv_create(&e, e.port);
	main_loop(&e);
	return (0);
}

#include "bircd.h"

void	main_loop(t_env *e)
{
	while (1)
	{
		init_fd(e);
		do_select(e);
		check_fd(e);
	}
}

#include "bircd.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

void	srv_accept(t_env *e, int s)
{
	int					cs;
	struct sockaddr_in	csin;
	socklen_t			csin_len;

	csin_len = sizeof(csin);
	cs = X(-1, accept(s, (struct sockaddr *)&csin, &csin_len), "accept");
	printf("New client #%d from %s:%d\n", cs, inet_ntoa(csin.sin_addr),
		ntohs(csin.sin_port));
	clean_fd(&e->fds[cs]);
	e->fds[cs].type = FD_CLIENT;
	e->fds[cs].fct_read = client_read;
	e->fds[cs].fct_write = client_write;
}

#include "bircd.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>

void	srv_create(t_env *e, int port)
{
	int					s;
	struct sockaddr_in	sin;
	struct protoent		*pe;

	pe = (struct protoent *)Xv(NULL, getprotobyname("tcp"), "getprotobyname");
	s = X(-1, socket(PF_INET, SOCK_STREAM, pe->p_proto), "socket");
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	X(-1, bind(s, (struct sockaddr *)&sin, sizeof(sin)), "bind");
	X(-1, listen(s, 42), "listen");
	e->fds[s].type = FD_SERV;
	e->fds[s].fct_read = srv_accept;
}

#include "bircd.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	x_int(int err, int res, char *str, char *file, int line)
{
	if (res == err)
	{
		fprintf(stderr, "%s error (%s, %d): %s\n", str, file, line,
			strerror(errno));
		exit(1);
	}
	return (res);
}

void	*x_void(void *err, void *res, char *str, char *file, int line)
{
	if (res == err)
	{
		fprintf(stderr, "%s error (%s, %d): %s\n", str, file, line,
			strerror(errno));
		exit(1);
	}
	return (res);
}
#ifndef BIRCD_H_
# define BIRCD_H_

# include <sys/select.h>

# define FD_FREE 0
# define FD_SERV 1
# define FD_CLIENT 2

# define BUF_SIZE 4096

# define Xv(err, res, str) (x_void(err, res, str, __FILE__, __LINE__))
# define X(err, res, str) (x_int(err, res, str, __FILE__, __LINE__))
# define MAX(a, b) ((a > b) ? a : b)

# define USAGE "Usage: %s port\n"

typedef struct s_fd
{
	int		type;
	void	(*fct_read)();
	void	(*fct_write)();
	char	buf_read[BUF_SIZE + 1];
	char	buf_write[BUF_SIZE + 1];
}			t_fd;

typedef struct s_env
{
	t_fd	*fds;
	int		port;
	int		maxfd;
	int		max;
	int		r;
	fd_set	fd_read;
	fd_set	fd_write;
}			t_env;

void		init_env(t_env *e);
void		get_opt(t_env *e, int ac, char **av);
void		main_loop(t_env *e);
void		srv_create(t_env *e, int port);
void		srv_accept(t_env *e, int s);
void		client_read(t_env *e, int cs);
void		client_write(t_env *e, int cs);
void		clean_fd(t_fd *fd);
int			x_int(int err, int res, char *str, char *file, int line);
void		*x_void(void *err, void *res, char *str, char *file, int line);
void		init_fd(t_env *e);
void		do_select(t_env *e);
void		check_fd(t_env *e);

#endif /* !BIRCD_H_ */
