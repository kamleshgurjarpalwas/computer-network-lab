#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#define MAX 80 
#define PORT 8080 
#define BACKLOG 5

// helper to ensure all bytes are sent
ssize_t send_all(int fd, const void *buf, size_t len) {
    size_t total = 0;
    const char *p = buf;
    while (total < len) {
        ssize_t n = send(fd, p + total, len - total, 0);
        if (n <= 0) return n; // error or connection closed
        total += n;
    }
    return total;
}

int main() {
  int sockfd, connfd;
  struct sockaddr_in servaddr, cliaddr;
  socklen_t cli_len = sizeof(cliaddr);
  int opt = 1;

  // 1.Server
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd == -1)
  {
    printf("socket creation failed...\n");
    exit(0);
  }
  else printf("Socket Successfully Created..\n");

  // 2. setsockopt(SO_REUSEADDR)
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt"); close(sockfd); exit(EXIT_FAILURE);
  }

  // 3. prepare address
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  servaddr.sin_port = htons(PORT);

  // 4. bind()
  if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind"); close(sockfd); exit(EXIT_FAILURE);
  }

  // 5. listen()
  if (listen(sockfd, BACKLOG) < 0) {
    perror("listen"); close(sockfd); exit(EXIT_FAILURE);
  }
  printf("Server listening on port %d\n", PORT);
  
  // 6. accept()
  connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &cli_len);
  if (connfd < 0) {
      perror("accept"); close(sockfd); exit(EXIT_FAILURE);
  }

  // Print client IP:port
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &cliaddr.sin_addr, client_ip, sizeof(client_ip));
  printf("Accepted connection from %s:%d\n", client_ip, ntohs(cliaddr.sin_port));

  // 7. send() — send greeting
  const char *greet = "Hello from Bharat!\n";
  if (send_all(connfd, greet, strlen(greet)) < 0) {
      perror("send");
  } else {
      printf("Greeting sent to Client.\n");
  }

  // 8. optionally read a response from client
  char buf[512];
  ssize_t n = recv(connfd, buf, sizeof(buf)-1, 0);
  if (n > 0) {
      buf[n] = '\0';
      printf("Received from client: %s\n", buf);
  } else if (n == 0) {
      printf("Client closed the connection.\n");
  } else {
      perror("recv");
  }

  // 9. close
  close(connfd);
  close(sockfd);
  return 0;
}