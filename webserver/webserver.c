#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define PORT 3005
#define BUFFER_SIZE 1024
char resp[] = "HTTP/1.0 200 OK\r\n"
        "Server: webserver-c\r\n"
        "Content-Type: text/html\r\n\r\n";
        // "<html>Hello world</html>\r\n";

#define GET "GET"
#define POST "POST"

void getRequest(char uri[], int client_sock) {
    printf("Get request received\n");
    if (!strcmp(uri, "/")) {
        FILE *fptr;
        fptr = fopen("index.html", "r");
        if (!fptr) {
            perror("File not found");
            write(client_sock, "HTTP/1.0 404 Not Found\r\n\r\n", 26);
            return;
        }
        const int size = 50000;
        char html[size];
        int i=0;
        char ch;
        while ((ch=fgetc(fptr)) != EOF && i<size-1) {
            html[i] = ch;
            i++;
        }
        fclose(fptr);
        char *fin_resp = (char*)malloc(strlen(resp) + strlen(html) + 1);
        strcpy(fin_resp, resp);
        strcat(fin_resp, html);
        // printf("%s\n", fin_resp);
        int writing = write(client_sock, fin_resp, strlen(fin_resp));
        if (writing < 0) {
            perror("webserver (write)");
        }
        free(fin_resp);
    }
}

void postRequest(char uri[]) {
	printf("Post request recieved!\n");
}

void *handle_client(void *client_sock_ptr) {
    char buffer[BUFFER_SIZE];
    int client_sock = *(int *)client_sock_ptr;
    free(client_sock_ptr);


    int valRead = read(client_sock, buffer, BUFFER_SIZE);
    if (valRead < 0) {
        perror("webserver (read)");
        close(client_sock);
        pthread_exit(NULL);
    }

    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, uri, version);
    if (!strcmp(method, POST)) {
        postRequest(uri);
    } else if (!strcmp(method, GET)) {
        getRequest(uri, client_sock);
    }
    printf("Received request: %s %s %s\n", method, uri, version);


    close(client_sock);
    pthread_exit(NULL);
}

int main() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        perror("webserver (socket)");
        return 1;
    }
    printf("Socket created successfully: %d\n", socketfd);

    int opt = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("webserver (setsockopt)");
        return 1;
    }

    struct sockaddr_in host_addr;
    socklen_t host_addrlen = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socketfd, (struct sockaddr *)&host_addr, host_addrlen) != 0) {
        perror("webserver (bind)");
        return 1;
    }
    printf("Socket bound successfully\n");

    if (listen(socketfd, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        return 1;
    }
    printf("Socket listening on port %d\n", PORT);

    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int *client_sock = malloc(sizeof(int));
        if (!client_sock) {
            perror("webserver (malloc)");
            continue;
        }

        *client_sock = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (*client_sock == -1) {
            perror("webserver (accept)");
            free(client_sock);
            continue;
        }

        printf("Connection accepted from %s:%u\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, client_sock) != 0) {
            perror("webserver (pthread_create)");
            close(*client_sock);
            free(client_sock);
            continue;
        }

        pthread_detach(client_thread);
    }

    close(socketfd);
    return 0;
}
