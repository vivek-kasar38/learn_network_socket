#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Basic socket structure
typedef struct {
    int state; // 0: unconnected, 1: connected, 2: disconnected
    char buffer[1024]; // Buffer for data transmission
} MySocket;

// Function to return memory location of socket
MySocket* create_socket() {
    MySocket* sock = (MySocket*)malloc(sizeof(MySocket));
    sock->state = 0;
    return sock;
}

// Basic UDP structure
typedef struct {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
} udp_header_t;

uint16_t custom_htons(uint16_t hostshort) {
    return (hostshort << 8) | (hostshort >> 8);
}


// Function to return memory location of UDP 
udp_header_t* construct_udp_header(uint16_t src_port, uint16_t dest_port, const char* data) {
    udp_header_t* header = (udp_header_t*)malloc(sizeof(udp_header_t));
    header->src_port = custom_htons(src_port); // Convert to network byte order
    header->dest_port = custom_htons(dest_port); // Convert to network byte order
    header->length = custom_htons(sizeof(udp_header_t) + strlen(data)); // Calculate and convert length to network byte order
    header->checksum = 0; // Initialize checksum, will be calculated later if needed

    return header;
}


void send_udp_packet(MySocket* sock, udp_header_t* header, const char* data) {
    // Copy header to buffer
    memcpy(sock->buffer, header, sizeof(udp_header_t));
    // Copy data after the header in the buffer
    memcpy(sock->buffer + sizeof(udp_header_t), data, strlen(data));

    // Change socket state to connected
    sock->state = 1;
}

void receive_udp_packet(MySocket* sock) {
    if (sock->state != 1) {
        printf("Socket is not connected!\n");
        return;
    }

    // Extract the UDP header from the buffer
    udp_header_t* header = (udp_header_t*)sock->buffer;
    // Extract the data
    char* data = sock->buffer + sizeof(udp_header_t);

    printf("Received packet:\n");
    printf("Source Port: %d\n", custom_htons(header->src_port)); // Convert to host byte order for printing
    printf("Destination Port: %d\n", custom_htons(header->dest_port)); // Convert to host byte order for printing
    printf("Data: %s\n", data);

    // Mark socket as disconnected
    sock->state = 2;
}

// Main
int main() {


    // Create and initialize the socket
    MySocket* sock = create_socket();

    // Construct the UDP header
    udp_header_t* header = construct_udp_header(8080, 9090, "Hello, UDP!,");

    // Send the UDP packet
    send_udp_packet(sock, header, "Hello, UDP!, I am here");

    // Receive the UDP packet
    receive_udp_packet(sock);

    // Clean up
    free(header);
    free(sock);

    return 0;
}
