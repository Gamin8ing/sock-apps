#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define PORT 8008

int clients[10];
int num_clients = 0;

void *handle_client(void *args) {
    int client_sockfd = *(int*)args;
    // reading a mssg
    for (;;) {

        char msg[20];
        int rec = recv(client_sockfd, msg, sizeof(msg), 0);
        if (rec == -1) {
            perror("recv");
            continue;
        }
        printf("Message received: %s\n", msg);
        for (int i=0; i<10; i++) {
            if (clients[i] == client_sockfd || clients[i] == 0) {
                continue;
            }
            int sent = send(clients[i], msg, sizeof(msg), 0);
            if (sent == -1) {
                perror("send");
                // return;
            }
            printf("Message sent to client %d\n", i);
            
        }
    }
    
    close(client_sockfd);
    // free(args);
    pthread_exit(NULL);
    // return 0;
}

int main() {
    // creating a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket create");
        return 1;
    }
    printf("Socket created successfully\n");

    // binding the socket
    struct sockaddr_in host_addr;
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int host_addrlen;
    host_addrlen = sizeof(host_addr);
    int bound = bind(sockfd, (struct sockaddr*)&host_addr, (socklen_t)host_addrlen);
    if (bound != 0) {
        perror("bind");
        return 1;
    }
    printf("Socket bound successfully\n");

    // listening for connections
    int listening = listen(sockfd, SOMAXCONN);
    if (listening != 0) {
        perror("listen");
        return 1;
    }
    printf("Listening for connections...\n");

    // accepting connections
    for (;;) {
        struct sockaddr_in client_addr;
        int client_addrlen;
        int client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);
        if (client_sockfd == -1) {
            perror("accept");
            return 1;
        }
        printf("Connection accepted\n");
        clients[num_clients++] = client_sockfd;
        
        pthread_t id;
        int th = pthread_create(&id, NULL, handle_client, (void*)&client_sockfd);
        if (th != 0) {
            perror("pthread_create");
            return 1;
        }
        pthread_detach(id);
        
    }
    for (int i = 0; i < 10; i++) {
        close(clients[i]);
    }
    close(sockfd);
}