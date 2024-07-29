#include "irc.h"

int main(void) {
	// Creating the client socket
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Defining server adress
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	// Connecting to the server
	connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	// Sending data to the server
	const char* message = "Hello, server!";
	send(clientSocket, message, strlen(message), 0);

	// Closing the client socket
	close(clientSocket);

	return (0);
}