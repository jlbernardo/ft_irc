
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
