This is a list of the functions allowed in the scope of the project. They may
or may not be used.

**socket**: Creates a new socket.

**close**: Closes an open file descriptor, including sockets.

**setsockopt**: Sets options on a socket.

**getsockname**: Retrieves the local name for a socket.

**getprotobyname**: Retrieves protocol information by name.

**gethostbyname**: Retrieves host information corresponding to a host name.

**getaddrinfo**: Translates the name of a service location to a set of socket addresses.

**freeaddrinfo**: Frees the memory allocated by `getaddrinfo`.

**bind**: Associates a socket with a local address.

**connect**: Establishes a connection to a specified socket.

**listen**: Marks a socket as a passive socket to accept incoming connections.

**accept**: Accepts a connection on a socket.

**htons**: Converts a short integer from host byte order to network byte order.

**htonl**: Converts a long integer from host byte order to network byte order.

**ntohs**: Converts a short integer from network byte order to host byte order.

**ntohl**: Converts a long integer from network byte order to host byte order.

**inet_addr**: Converts an IPv4 address from the dotted-decimal format to an integer.

**inet_ntoa**: Converts an IPv4 address from an integer to the dotted-decimal format.

**send**: Sends data on a connected socket.

**recv**: Receives data from a connected socket.

**signal**: Sets a signal handler for a specific signal.

**sigaction**: Examines and changes a signal action.

**lseek**: Repositions the offset of an open file descriptor.

**fstat**: Retrieves status information about a file.

**fcntl**: Performs various operations on a file descriptor.

**poll**: Waits for events on file descriptors.
