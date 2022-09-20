#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define DIM 1024
#define REMOTE_PORT 5555
#define LISTENING_PORT 5555
#define BACKLOG 1

int main(int argc, char * argv[]) {
    int sockfd, newsockfd, n;
    struct sockaddr_in localaddr, remoteaddr;
    socklen_t len = sizeof(struct sockaddr);
    char msg[DIM];
    FILE * files;

    printf("[+] Apertura della socket...\n");
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("setsockopt()");
        exit(1);
    }

    memset(&localaddr, 0, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(LISTENING_PORT);
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    printf("[+] Effettuo operazione di bind...\n");
    if (bind(sockfd, (struct sockaddr *) &localaddr, len) < 0) {
        perror("bind()");
        exit(1);
    }

    memset(&remoteaddr, 0, sizeof(remoteaddr));
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(REMOTE_PORT);

    listen(sockfd, BACKLOG);
    printf("[+] In attesa di connessione da parte del client...\n");
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &remoteaddr, &len);
        printf("[+] Accetto il tentativo di connessione da parte del client...\n");
        if (!fork()) {
            close(sockfd);

            while (1) {
                printf("[+] In attesa di richiesta da parte del client...\n");
                n = recv(newsockfd, msg, DIM - 1, 0);
                msg[n] = 0;

                printf("[+] Hai ricevuto un messaggio!\n");
                printf("[%s] : %s\n", inet_ntoa(remoteaddr.sin_addr), msg);

                if (strstr(msg, "list") != NULL) {
                    if ((files = fopen("list.txt", "r")) == NULL) {
                        perror("fopen()");
                        exit(1);
                    }
                    char line[DIM];
                    while (fgets(line, DIM, files) != NULL)
                        strcat(msg, line);
                    send(newsockfd, msg, strlen(msg), 0);
                    printf("[+] Messaggio inviato!\n");
                } else {
                    if ((files = fopen(msg, "r")) == NULL) {
                        printf("[+] File inesistente!\n");
                    } else {
                        while (fgets(msg, DIM - 1, files) != NULL) { // Reading and sending line by line
                            send(newsockfd, msg, strlen(msg), 0);
                            printf("[+] Messaggio inviato!\n");
                            sleep(1);
                        }
                    }
                    send(newsockfd, "EOF", 4, 0);
                }
            }
        } else {
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}