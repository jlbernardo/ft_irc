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

	// Listen for the connection
	listen(serverSocket, 5); // We then tell the application to listen to the socket refffered by the serverSocket.

	// Accepting client connections
	int clientSocket = accept(serverSocket, NULL, NULL); // The accept() call is used to accept the connection request that is received on the socket the application was listening to.
	// (void)clientSocket;
	// Receiving data from the client
	char buffer[1024] = {0};
	println(CYAN << recv(clientSocket, buffer, sizeof(buffer), 0));
	// println(GREEN << "Message from client: " << buffer);
	std::cout << RED << "Message from client: " << buffer << std::endl;
	// Closing server socket
	close(serverSocket);
	return(0);
}
