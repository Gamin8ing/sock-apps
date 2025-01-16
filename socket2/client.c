#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define CLIENT_PORT 8080
#define	MYHOST	((in_addr_t) 0x983bb978)


void *receive_msgs(void* sockfd) {
    int client_sockfd = *(int*)sockfd;
    printf("listening for msg\n");
    for (;;) {
        char msg[20];
        int rec = recv(client_sockfd, msg, sizeof(msg), 0);
        if (rec == -1) {
            perror("recv");
            continue;
        }
        printf("%s", msg);
    }
}

int main(int argc, char *argv[]) {
    // creating a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket create");
        return 1;
    }
    printf("Socket created successfully\n");
    int port = atoi(argv[1]);

    // binding the socket
    struct sockaddr_in host_addr;
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(port);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int host_addrlen;
    host_addrlen = sizeof(host_addr);
    int bound = bind(sockfd, (struct sockaddr*)&host_addr, (socklen_t)host_addrlen);
    if (bound != 0) {
        perror("bind");
        return 1;
    }
    printf("Socket bound successfully\n");

    // connect to the server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8008);
    // server_addr.sin_addr.s_addr = htonl(MYHOST);
    server_addr.sin_addr.s_addr = inet_addr("34.93.221.142");
    int server_addrlen;
    server_addrlen = sizeof(server_addr);

    int con = connect(sockfd, (struct sockaddr*)&server_addr, (socklen_t)server_addrlen);
    if (con != 0) {
        perror("connect");
        return 1;
    }
    printf("Connected to the server\n");

    // creating a thread for receiving messages
    pthread_t recv_thread;
    int recv_thread_create = pthread_create(&recv_thread, NULL, receive_msgs, (void*)&sockfd);
    if (recv_thread_create != 0) {
        perror("pthread_create");
        return 1;
    }
    // pthread_detach(recv_thread);
    printf("Thread created successfully\n");
    for (;;) {
        // send a message to the server
        // printf("Enter a message: ");
        char msg[20];
        // scanf("%s", msg);
        fgets(msg, sizeof(msg), stdin);
        // printf("%d\n", strcmp(msg, "exit"));
        if (strcmp(msg, "exit") == 0) {
            break;
        }
        int sent = send(sockfd, msg, sizeof(msg), 0);
        if (sent == -1) {
            perror("send");
            return 1;
        }
        // printf("Message sent\n");

    }

    // close(con);
    fflush(stdin);
    close(sockfd);
    return 0;
}