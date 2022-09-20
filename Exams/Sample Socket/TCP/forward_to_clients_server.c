#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define REMOTE_PORT 5656
#define LISTENING_PORT 5656
#define DIM 1024

char * make_msg(char username[], char msg[]) {
    char * _ = malloc(DIM);

    strcpy(_, username);
    strcat(_, " : ");
    strcat(_, msg);

    printf("Stringa finale = %s", _);
    return _;
}

void forward(char addr[], char msg[]) {
    int sockfd;
    struct sockaddr_in localaddr, remoteaddr;
    socklen_t len = sizeof(struct sockaddr_in);

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    printf("[+] Setup remote address parameters...\n");
    memset(&remoteaddr, 0, sizeof(remoteaddr));
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(REMOTE_PORT);
    inet_pton(AF_INET, addr, &(remoteaddr.sin_addr));

    if (connect(sockfd, (struct sockaddr *) &remoteaddr, sizeof(remoteaddr)) < 0) {
        perror("connect()");
        exit(1);
    }

    printf("[+] Connected to %s\n", addr);
    send(sockfd, msg, strlen(msg), 0);
    printf("[+] Forward message to %s\n", addr);

    close(sockfd);
}

int main(int argc, char * argv[]) {
    int sockfd, newsockfd, n;
    struct sockaddr_in localaddr, remoteaddr;
    socklen_t len = sizeof(struct sockaddr_in);
    FILE * IP; // Save ip address within a file
    char msg[DIM], username[32], line[DIM], addr[16];

    if ((IP = fopen("address.txt", "w+")) == NULL) {
        perror("fopen()");
        exit(1);
    }

    printf("[+] Socket opening...\n");
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    printf("[+] Setup local address parameters...\n");
    memset(&localaddr, 0, sizeof localaddr);
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(LISTENING_PORT);
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("[+] Binding...\n");
    if (bind(sockfd, (struct sockaddr *) &localaddr, len) < 0) {
        perror("bind()");
        exit(1);
    }

    printf("[+] Setup remote address parameters...\n");
    memset(&remoteaddr, 0, sizeof(remoteaddr));
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(REMOTE_PORT);

    printf("[+] Listening...\n");
    listen(sockfd, 10);

    while (1) {
        len = sizeof remoteaddr;
        newsockfd = accept(sockfd, (struct sockaddr *) &remoteaddr, &len);
        printf("[+] Accepts connection attempt by %s...\n", inet_ntoa(remoteaddr.sin_addr));

        printf("[+] Write %s inside the file...\n", inet_ntoa(remoteaddr.sin_addr));
        char _IP[16];
        strcpy(_IP, inet_ntoa(remoteaddr.sin_addr));
        strcat(_IP, "\n");
        fputs(_IP, IP);
        fflush(IP);

        if (!fork()) {
            close(sockfd);

            // Get username
            if ((n = recv(newsockfd, username, DIM - 1, 0)) <= 0) 
                return 0;
            username[n - 1] = '\0';
            printf("[%s] : %s", inet_ntoa(remoteaddr.sin_addr), username);
            
            while (1) {
                // Get messages
                if ((n = recv(newsockfd, msg, DIM - 1, 0)) <= 0) 
                    return 0;
                msg[n] = 0;
                printf("[%s] : %s", inet_ntoa(remoteaddr.sin_addr), msg);     

                // Forward received message to other connected clients 
                rewind(IP);
                while (fgets(line, DIM, IP) != NULL) {
                    if (strstr(line, inet_ntoa(remoteaddr.sin_addr)) == NULL) {
                        if (!fork()) {
                            strcpy(addr, line);
                            addr[strlen(addr) - 1] = '\0';

                            forward(addr, make_msg(username, msg));
                        }                            
                    } 
                }
            }
        } else {
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}