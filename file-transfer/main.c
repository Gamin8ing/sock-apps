#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <error.h>
#include <unistd.h>

char buffer[1024];     // the buffer to be stored in the data transmission
char recvbuffer[1024]; // the buffer for when the data is received

void HandleSender(int filedes) {
  int sockfd = filedes;
  printf("Enter the name of the file to be sent: ");
  char filename[100];
  scanf("%s", filename);
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    perror("File opening");
    return;
  }
  while (fread(buffer, sizeof(char), 1024, file) > 0) {
    // send the buffer to the receiver
    send(sockfd, buffer, sizeof(buffer), 0);
    printf("writing..%s\n", buffer);
  }
  fclose(file);
  close(sockfd);
  return;
}

int main(int argc, char **argv) {
  // printf("hello world");
  // sockets work as:
  // creation -> binding -> listening -> accepting -> read/write

  // creating a listening socket for accepting connections
  int lsockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (lsockfd == -1) {
    perror("Listening Socket creation");
    return 1;
  }
  // creating a passive socket for initiating connections
  int psockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (psockfd == -1) {
    perror("Passive Socket creation");
    return 1;
  }
  printf("Both sockets created successfuly!\n");

  int PORT = 8100;
  printf("Enter your socket port address (Listening): ");
  scanf("%d", &PORT);
  struct sockaddr_in l_addr, p_addr;
  socklen_t l_addr_len, p_addr_len;
  l_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  l_addr.sin_port = htons(PORT);
  l_addr.sin_family = AF_INET;
  l_addr_len = sizeof(l_addr);
  printf("Enter your socket port address (Passive): ");
  scanf("%d", &PORT);
  p_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  p_addr.sin_port = htons(PORT);
  p_addr.sin_family = AF_INET;
  p_addr_len = sizeof(p_addr);

  if (bind(lsockfd, (struct sockaddr *)&l_addr, l_addr_len) == -1) {
    perror("Listening Socket binding");
    return 1;
  }
  if (bind(psockfd, (struct sockaddr *)&p_addr, p_addr_len) == -1) {
    perror("Passive Socket binding");
    return 1;
  }
  printf("Both Socket Bound Successfuly!\n");

  // listening socket up for listening
  // listening for incoming connections
  if (listen(lsockfd, 10)) {
    perror("Listening Socket listening");
    return 1;
  }
  printf("Socket Listening Successfuly!\n");

  int identity = 0;
  int CLIENT_PORT = 8101;
  // 0 for receiver and 1 for sender
  printf("Are you a sender or a receiver? (1/0): ");
  scanf("%d", &identity);

  // receiver side code
  if (identity == 0) {
    while (1) { // address of the peer connection
      struct sockaddr_in client_addr;
      socklen_t client_addr_len;
      int client_sockfd;
      client_sockfd =
          accept(lsockfd, (struct sockaddr *)&client_addr, &client_addr_len);

      if (client_sockfd == -1) {
        perror("Listenng Socket accepting");
        continue;
      }
      printf("Connection accepted!\n");
      while (1) {
        memset(recvbuffer, 0, sizeof(recvbuffer));
        int re = recv(client_sockfd, recvbuffer, sizeof(recvbuffer), 0);
        if (re > 0) {
          printf("Reading something..\n");
          FILE *wfile = fopen("data", "ab");
          // writing the data to the file
          fwrite(recvbuffer, sizeof(char), 1024, wfile);
          fclose(wfile);
        } else if (re == 0) {
          printf("Connection closed! and data written to data\n");
          close(client_sockfd);
          close(lsockfd);
          return 0;
        }
        // } else /* if (recv(lsockfd, recvbuffer, siz) == 0)  */ {
        //   printf("Connection closed!\n");
        //   break;
        // }
      }
    }
    close(lsockfd);
  } else { // sender side code
    printf("Enter the port number of the recevier: ");
    scanf("%d", &CLIENT_PORT);
    struct sockaddr_in client_addr2;
    socklen_t client_addr_len2 = sizeof(client_addr2);
    // client_addr2.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_addr2.sin_addr.s_addr = inet_addr("172.22.80.179");
    client_addr2.sin_port = htons(CLIENT_PORT);
    client_addr2.sin_family = AF_INET;

    if (connect(psockfd, (struct sockaddr *)&client_addr2, client_addr_len2) ==
        -1) {
      perror("Passive Socket connecting");
      return 1;
    }
    printf("Connected to the peer!\n");
    HandleSender(psockfd);
  }
  close(psockfd);
  close(lsockfd);
  return 0;
}
