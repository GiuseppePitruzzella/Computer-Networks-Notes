// Simple TCP Server
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

int main(int argc, char * argv[]) {
    int sockfd, newsockfd, n;
    struct sockaddr_in localaddr, remoteaddr;
    socklen_t len = sizeof(struct sockaddr_in);
    char msg[DIM];

    printf("[+] Validation of input parameters...\n");
    if (argc < 2) {
        printf("Parametri insufficienti! Usa <REMOTE_IP>\n");
        return -1;
    }

    if (!fork()) {
        printf("[+] Socket opening...\n");
        if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket()");
            exit(1);
        }

        // look at this!!!
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
            perror("setsockopt(SO_REUSEADDR) failed");

        printf("[+] Setup remote address parameters...\n");
        memset(&remoteaddr, 0, sizeof(remoteaddr));
        remoteaddr.sin_family = AF_INET;
        remoteaddr.sin_port = htons(REMOTE_PORT);
        inet_pton(AF_INET, argv[1], &(remoteaddr.sin_addr));

        while (connect(sockfd, (struct sockaddr *) &remoteaddr, sizeof(remoteaddr)) < 0) {
            sleep(1);
        }
        printf("[+] Connected to server...\n");

        while (fgets(msg, DIM, stdin) != NULL) {
            send(sockfd, msg, strlen(msg), 0);
            printf("[+] Send message to server...\n");
        }
    } else {
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

            if (!fork()) {
                close(sockfd);

                while (1) {
                    if ((n = recv(newsockfd, msg, DIM - 1, 0)) <= 0) 
                        return 0;
                    msg[n] = 0;
                    printf("[%s] : %s", inet_ntoa(remoteaddr.sin_addr), msg);     
                }
            } else {
                close(newsockfd);
            }                
        }
    }
    printf("[+] Exit...");
    close(sockfd);
    return 0;
}