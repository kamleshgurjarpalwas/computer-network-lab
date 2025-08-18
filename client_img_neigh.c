#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_IP "192.168.0.209"
#define PORT 8080
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    FILE *image_file;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error("ERROR opening socket");
    }

    // Set server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        error("ERROR invalid address or address not supported");
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("ERROR connecting to server");
    }

    // Open the file to send
    image_file = fopen("sending_img.png", "rb");
    if (image_file == NULL) {
        error("ERROR opening file to send");
    }

    // Read from the file and send it to the server
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, image_file)) > 0) {
 
        if (send(sock, buffer, bytes_read, 0) < 0) {
            error("ERROR sending data");
        }
    }

    printf("Image sent successfully.\n");

    // Close the file and socket
    fclose(image_file);
    close(sock);

    return 0;
}
