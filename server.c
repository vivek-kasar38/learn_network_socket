#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address on the machine
    address.sin_port = htons(PORT); // Convert port number to network byte order

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }


    while(1) {
        // Listen for incoming connections
        if (listen(server_fd, 3) < 0) {
            perror("listen");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Accept an incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Read the request
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes_read > 0) {
            printf("Received request: %s\n", buffer);

            // Prepare the HTML response
            char *html_content = "<!DOCTYPE html>"
                                 "<html>"
                                 "<head><title>My Simple Server</title></head>"
                                 "<body><h1>Hello, World!</h1>"
                                 "<p>This is a simple HTML response from your server.</p>"
                                 "</body></html>";
            char response[BUFFER_SIZE];

            // Create the HTTP response headers
            snprintf(response, BUFFER_SIZE,
                     "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/html\r\n"
                     "Content-Length: %zu\r\n"
                     "\r\n%s",
                     strlen(html_content), html_content);

            // Send the HTTP response with the HTML content
            write(new_socket, response, strlen(response));
        }

        // Close the connection to this client
        close(new_socket);
    }

    // The server_fd is never closed here because the server is meant to run indefinitely.
    return 0;
}


