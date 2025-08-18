#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    FILE *received_file;
    int bytes_read;

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        error("ERROR opening socket");
    }

    // Set server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("ERROR on binding");
    }

    // Listen for incoming connections
    listen(server_sock, 5);
    client_len = sizeof(client_addr);

    // Accept client connection
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0) {
        error("ERROR on accept");
    }

    // Open the file to save the received image
    received_file = fopen("received_img.png", "wb");
    if (received_file == NULL) {
        error("ERROR opening file to write");
    }

    // Receive data from client and write to file
    while ((bytes_read = recv(client_sock, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_read, received_file);
    }

    if (bytes_read < 0) {
        error("ERROR reading from socket");
    }

    printf("Image received successfully.\n");

    // Close connections and files
    close(client_sock);
    close(server_sock);
    fclose(received_file);

    return 0;
}
