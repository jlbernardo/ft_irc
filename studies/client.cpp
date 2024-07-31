#include "irc.h"

int main(void) {
	// Creating the client socket
	int test = socket(AF_INET, SOCK_STREAM, 0);

	// Defining server adress
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	// Connecting to the server
	connect(test, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	// Sending data to the server
	const char* message = "Hello, server!";
	send(test, message, strlen(message), 0);

	// Closing the client socket
	close(test);

	return (0);
}