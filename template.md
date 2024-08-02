# FT_IRC TED talks

## Table of contents

- [Server-client dynamics](#Server-client-dynamics)
  - [Socket()](#socket)
  - [Bind()](#bind)
  - [Listen()](#listen)
  - [Accpet()](#accept)
- [sockaddr_in](#sockaddr-in)
	- [sin_family](#sin-family)
	- [sin_addr.s_addr](#sin-addr.s-addr)
	- [sin_port](#sin-port)
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


```cpp
	int clientSocket = accept(serverSocket, NULL, NULL);
```
## Struct sockaddr_in

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

```cpp
	serverAddress.sin_family = AF_INET;
```
### sin_addr.s_addr

```cpp
	serverAddress.sin_addr.s_addr = INADDR_ANY;
```
### sin_port

```cpp
	serverAddress.sin_port = htons(8080);
```


## I/O Multiplexing

### select()

### poll()

### epoll()
