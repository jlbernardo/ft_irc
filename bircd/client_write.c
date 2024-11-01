
#include "bircd.h"
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>

void	client_write(t_env *e, int cs)
{
	int	len;

	printf("called client_write\n");
	printf("buf before send: %s\n", e->fds[cs].buf_write);
	len = strlen(e->fds[cs].buf_write);
	if (len > 0)
	{
		send(cs, e->fds[cs].buf_write, len, 0);
		e->fds[cs].buf_write[0] = '\0';
		printf("buf after send: %s\n", e->fds[cs].buf_write);
	}
}
