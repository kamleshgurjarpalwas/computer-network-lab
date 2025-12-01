#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buf[512];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket"); return 1; }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "192.168.0.244", &servaddr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    // receive greeting
    ssize_t n = recv(sockfd, buf, sizeof(buf)-1, 0);
    if (n > 0) {
        buf[n] = '\0';
        printf("Server says: %s", buf);
    } else if (n == 0) {
        printf("Server closed connection\n");
    } else {
        perror("recv");
    }

    // optionally send reply
    const char *reply = "Hello server, I got your message!\n";
    send(sockfd, reply, strlen(reply), 0);

    close(sockfd);
    return 0;
}
