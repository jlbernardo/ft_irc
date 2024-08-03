# FT_IRC TED talks

## Table of contents

- [Server-client dynamics](#Server-client-dynamics)
  - [Socket()](#socket)
  - [Bind()](#bind)
  - [Listen()](#listen)
  - [Accpet()](#accept)
- [sockaddr_in](#sockaddr_in)
	- [sin_family](#sin_family)
	- [sin_addr.s_addr](#sin_addr.s_addr)
	- [sin_port](#sin_port)
- [I/O Multiplexing](#I/O-Multiplexing)
  - [select()](#select)
  - [poll()](#poll)
  - [epoll()](#epoll)

## Overview

### Screenshot

![](./images/mobile-design.jpg)	

### Links

- Solution URL: [Github](https://github.com/allfigueiredodev/ip-adress-tracker.git)
- Live Site URL: [Github Pages](https://allfigueiredodev.github.io/ip-adress-tracker/)

### Useful resources

- [LeafletJS](https://leafletjs.com/) - An open-source JavaScript library for mobile-friendly interactive maps.
- [Ipify](https://geo.ipify.org/) - IP Geolocation API allows you to locate and identify website visitors by IP address. IP location helps prevent fraud, customize web experiences, and maintain regulatory compliance.

## Server-client dynamics

### Socket()

```cpp
	int socket(int domain, int type, int protocol);
```
socket() creates an endpoint for communication and returns a file
descriptor that refers to that endpoint.  The file descriptor
returned by a successful call will be the lowest-numbered file
descriptor not currently open for the process.

- domain

	The domain argument specifies a communication domain. This
	selects the protocol family which will be used for communication.
	Valid values for this field include AF_INET, AF_INET6 and many others
	defined in <sys/socket.h>. In our case, we will be using AF_INET to work with
	a IPv4 protocol socket.
	
- type

	Type, specifies the communication semantics. E.g. SOCK_STREAM or SOCK_DGRAM.
	In our project we will be using SOCK_STREAM due its characteristics that are:
	
	Sequenced: Data sent over a SOCK_STREAM socket is sequenced, meaning it arrives in the order it was sent.

	Reliable: SOCK_STREAM ensures data integrity and delivery. If a packet gets lost during transmission, mechanisms are in place to detect this loss and retransmit the missing data.

	Two-way: This indicates that the communication channel established by the socket allows for bidirectional data flow. Both ends of the connection can send and receive data simultaneously, facilitating interactive applications like chat programs or video calls.

	Connection-based: Unlike connectionless protocols (like UDP, AKA: SOCK_DGRAM), SOCK_STREAM sockets establish a persistent connection between the sender and receiver before data transfer begins. This connection setup involves a handshake process to ensure both parties are ready to communicate, adding an extra layer of reliability.

	Byte streams: Data transmitted over a SOCK_STREAM socket is treated as a continuous stream of bytes. The application layer defines how data should be segmented or interpreted. This allows for flexible data handling but requires careful management of data framing at the application level.

- protocol

	The protocol specifies a particular protocol to be used with the socket. Normally only a single protocol exists to support a particular socket type within a given protocol family, in which 
	case protocol can be specified as 0.

Given the above information, our server socket will be created using the socket() function as follows:

```cpp
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0)
```

### bind()

```cpp
	int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

When a socket is created with socket(), it exists in a name
space (address family) but has no address assigned to it.  bind()
assigns the address specified by addr to the socket referred to
by the file descriptor sockfd.

- sockfd
	
	The previously created socket with the socket() call.

- addr

	The struct containing data related to the adress that will be bound to the socket,
	more information about the sockaddr_in below.  

- addrlen

	Addrlen specifies the size, in bytes, of the address structure pointed to by addr.

Given the above information we will call bind function as follows:

```cpp
	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
```
### listen()

```cpp
	int listen(int fd, int backlog_queue_size);
```

Once a server has been bound to an address, the server can then call listen() on the socket.
The parameters to this call are the socket (fd) and the maximum number of queued connections requests up to backlog_queue_size.

```cpp
	listen(serverSocket, 5);
```

### accept()

```cpp
	int accept(int fd, struct sockaddr *remote_host, socklen_t addr_length);
```

Once a socket is listening for connections, the accept() function is used to accept incoming connection requests. Each time a client connects to the server, accept() waits until a client request arrives and then creates a new socket specifically for communicating with that client. This allows the server to handle multiple clients simultaneously, each with its own dedicated socket.

The original listening socket remains open to accept further incoming connections, while the new socket returned by accept() is used for sending and receiving data to/from the connected client.

- fd
	The same socket created by the socket() call.

- remote_host
	This parameter can be nulled if we do want to store data about the client side, in our case we will need to do that, so we will have to declare another addr struct in order to save the clients info.

- addr_length
	 holds the size of the addr structure. On return, this variable holds the actual length (in bytes) of the address stored in addr, if the remote_host parameter is null, this also has to be.

```cpp
	struct sockaddr_in clientAddress;
	socklen_t clientAddrLen = sizeof(clientAddress);

	int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddrLen);
	if (clientSocket == -1) {
		perror("accept failed");
		exit(EXIT_FAILURE);
}
```

## Struct sockaddr_in

Describes an IPv4 Internet domain socket address. The sin_port and sin_addr
members are stored in network byte order.

```cpp
	struct sockaddr_in {
		sa_family_t     sin_family;     /* AF_INET */
		in_port_t       sin_port;       /* Port number */
		struct in_addr  sin_addr;       /* IPv4 address */
	};
```

```cpp
	sockaddr_in serverAddress;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(8080);
```

### sin_family

Describes a socket's protocol family. This is an unsigned integer type. We will
be using AF_INET, which is the macro that holds the IPv4 prtocol family, [there are many other 
protocols](https://man7.org/linux/man-pages/man7/address_families.7.html) that could be used, but as sockaddr_in is made to work with IPv4, AF_INET is the right choice.

```cpp
	serverAddress.sin_family = AF_INET;
```

### sin_addr.s_addr

sin_addr is the IP host address.  The s_addr member of struct
in_addr contains the host interface address in network byte
order. INADDR_ANY (0.0.0.0) means any address for socket binding;

```cpp
	serverAddress.sin_addr.s_addr = INADDR_ANY;
```
### sin_port

sin_port contains the port in network byte order.  The
port numbers below 1024 are called privileged ports (or
sometimes: reserved ports). 

```cpp
	serverAddress.sin_port = htons(8080);
```

## I/O Multiplexing

I/O multiplexing is a technique used in computer networking to allow a single process to monitor multiple input/output channels at once. It's particularly useful in server applications that need to handle many client connections concurrently without dedicating a separate thread or process for each one. This efficiency is achieved by having a single thread or process wait for activity on any of the monitored channels and then dispatching the appropriate handler for that channel.

### How I/O Multiplexing Works

I/O multiplexing works by polling multiple file descriptors (sockets, pipes, etc.) in a loop, checking if any of them have data ready to read or write. If a file descriptor indicates readiness, the corresponding handler is invoked to process the data. This mechanism allows a single thread to manage multiple connections, significantly reducing the overhead associated with creating and destroying threads or processes for each connection.

### select()
	A system call available on Unix-like operating systems that monitors multiple file descriptors to see if they're ready for reading, writing, or have an exceptional condition pending.

### poll()
	Similar to select(), but can handle a larger number of file descriptors and provides more information about each descriptor.

### epoll()
	Linux-specific, offers more scalability and efficiency than select() and poll() by using edge-triggered notifications instead of level-triggered ones.