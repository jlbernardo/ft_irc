#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024];
    int port = 6668;
    std::vector<int> clients;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    fd_set master_set, read_set;
    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    int max_fd = server_fd;

    while (1) {
        read_set = master_set;
        if (select(max_fd + 1, &read_set, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        for (int fd = 0; fd <= max_fd; fd++) {
            if (!FD_ISSET(fd, &read_set))
                continue;

            if (fd == server_fd) {
                // New connection
                int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }

                FD_SET(client_fd, &master_set);
                if (client_fd > max_fd)
                    max_fd = client_fd;
                clients.push_back(client_fd);
            } else {
                // Client activity
                ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
                if (bytes_read <= 0) {
                    // Client disconnected
                    FD_CLR(fd, &master_set);
                    close(fd);
                    std::vector<int>::iterator it = std::find(clients.begin(), clients.end(), fd);
                    if (it != clients.end())
                        clients.erase(it);
                    continue;
                }

                buffer[bytes_read] = '\0';
                std::stringstream str;
                str << "Client " << fd << " says: " << buffer;

                // Broadcast to other clients
                for (size_t i = 0; i < clients.size(); ++i) {
                    if (clients[i] != fd) {
                        send(clients[i], str.str().c_str(), str.str().length(), 0);
                    }
                }
            }
        }
    }

    // Cleanup
    close(server_fd);
    for (size_t i = 0; i < clients.size(); ++i) {
        close(clients[i]);
    }
}
