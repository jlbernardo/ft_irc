#include "irc.h"

int main(void) {
	// Creating the server socket
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // socket() will return an fd, which will be used as the server socket

	// Creating the server adress
	sockaddr_in serverAddress;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080); // This function is used to convert the unsigned int from machine byte order to network byte order.
	serverAddress.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY stands for (0.0.0.0), means any address for socket binding.

	// Binding the server socket 
	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	// poll();

	// Listen for the connection
	listen(serverSocket, 5); // We then tell the application to listen to the socket refffered by the serverSocket.

	// Accepting client connections
	int clientSocket = accept(serverSocket, NULL, NULL); // The accept() call is used to accept the connection request that is received on the socket the application was listening to.
	// (void)clientSocket;
	// Receiving data from the client
	char buffer[1024] = {0};
	
	// println(CYAN << recv(clientSocket, buffer, sizeof(buffer), 0));
	println(GREEN << "Message from client: " << buffer);

	//poll() implementation:

	/* struct pollfd definition:
		struct pollfd {
		int   fd;          // file descriptor 
 		short events;      // requested events
		short revents;     // returned events 
		};
	*/

	struct pollfd fds[5];
	int num_clients = 0;

	while (1) {
		fds[num_clients].fd = serverSocket;
		fds[num_clients].events = POLLIN;
		num_clients++;

		for(int i = 0; i < num_clients; i++) {
			if (fds[i].events && POLLIN) {
				int client_fd = accept(fds[i].fd, NULL, NULL);
				if (client_fd >= 0) {
					fds[num_clients].fd = client_fd;
					fds[num_clients].events = POLLIN;
					num_clients++;
				}
			}
		}
		int ret = poll(fds, num_clients, -1);

		for (int i = 0; i < num_clients; i++) {
			if (fds[i].revents & POLLIN) {
				char buffer[1024];
				size_t bytes_read = recv(fds[i].fd, buffer, 1024 - 1, 0);
				if (bytes_read > 0) {
					buffer[bytes_read] = '\0';
					println(GREEN << "Received message: " << buffer);
					for (int j = 0; j < num_clients; i++) {
						if (fds[j].revents & POLLIN) {
							send(fds[j].fd, buffer, strlen(buffer), 0);
						}
					}
				} else if (!bytes_read) {
					close(fds[i].fd);
					for (int j = i; j < num_clients - 1; j++) {
						fds[j] = fds[j + 1];
					}
					num_clients--;
				}
			}
		}

	}

	// select() implementation:

// 	while (1) {
// 		size_t last_fd = serverSocket;
// 		fd_set read_fds;
// 		FD_ZERO(&read_fds);
// 		FD_SET(serverSocket, &read_fds);

// 		for (size_t i = 0; i < 2; i++) {
// 			int client_fd = accept(serverSocket, NULL, NULL);
// 			if (client_fd > 0) {
// 				FD_SET(client_fd, &read_fds);
// 				last_fd = client_fd;
// 			}
// 		}
// 		select(last_fd + 1, &read_fds, NULL, NULL, NULL);

// 		for (size_t i  = 0; i <= last_fd; i++) {
// 			if (FD_ISSET(i, &read_fds)) {
// 				char buffer[256];
// 				size_t bytes_read = recv(i, buffer, 256 - 1, 0);
// 				if (bytes_read > 0) {
// 					buffer[256] = '\0';
// 					println(GREEN << "Received message: " << buffer);
// 					for (size_t j = 0; j < 2; j++) {
// 						if (FD_ISSET(j, &read_fds)) {
// 							send(j, buffer, strlen(buffer), 0);
// 						}
// 					}
// 				} else if (!bytes_read) {
// 					close(i);
// 					FD_CLR(i, &read_fds);
// 				}
// 			}
// 		}
// 	}
// 	std::cout << RED << "Message from client: " << buffer << std::endl;
// 	// Closing server socket
// 	// close(serverSocket);
// 	// close(test);

// 	return(0);
// }
