#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define DIM 1024
#define _TOKEN_DIM 32
#define REMOTE_PORT 5552
#define LISTENING_PORT 5552

#define MYADDR "192.268.56.107"

int main(int argc, char *argv[]) {
    int sockfd, n;
    struct sockaddr_in localaddr, serveraddr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[DIM], text[DIM - 32], _[32];
    int _token;

    if (argc < 2) {
        printf("Errore! Parametri insufficienti!\nUsa <SERVER_IP>\n");
        return -1;
    }

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Errore durante l'apertura della socket!\n");
        return -1;
    }

    memset(&serveraddr, 0, len);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(REMOTE_PORT);
    inet_pton(AF_INET, argv[1], &(serveraddr.sin_addr));

    memset(&localaddr, 0, len);
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(LISTENING_PORT);

    if (bind(sockfd, (struct sockaddr *) &localaddr, len) < 0) {
        printf("Errore durante il binding!\n");
        return -1;
    }

    // Setting up struct
    bzero(buffer, DIM);
    printf("Your IP Address: ");
    scanf("%s", _); // Your IP Address
    strncat(_, "\0", strlen(_));
    strcat(buffer, _);

    bzero(_, 32);
    strcat(buffer, " ");

    printf("Password: ");
    scanf("%s", _);
    strncat(_, "\0", strlen(_));
    strcat(buffer, _);

    bzero(_, 32);
    strcat(buffer, " ");

    printf("Porta: ");
    scanf("%s", _);
    strncat(_, "\0", strlen(_));
    strcat(buffer, _);

    strcat(buffer, "\0");
    printf("Invio '%s'...\n", buffer);
    
    // Invio del messaggio per la registrazione al serverj
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        printf("Errore durante l'invio del messaggio di login!\n");
        return -1;
    }

    if ((n = recvfrom(sockfd, &_token, 32, 0, (struct sockaddr *) &serveraddr, &len)) < 0) {
        printf("Errore durante la ricezione del _token!\n");
        return -1;        
    }

    printf("Hai ricevuto un messaggio!\n");
    if (_token > 0) {
        printf("Token: %i\n", _token);
    } else {
        printf("Error! Username already exists!\n");
        return -1;
    }
    
    char token[32];
    sprintf(token, "%d", _token);
    sleep(2);

    if (!fork()) {
        while (1) {
            bzero(buffer, DIM);
            bzero(text, DIM - 32);

            printf("Message: ");
            scanf("%s", text);
            // fgets(text, DIM - 32, stdin);

            strcat(buffer, token);
            strcat(buffer, " ");
            strcat(buffer, text);

            if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
                printf("Errore durante l'invio del messaggio di login!\n");
                return -1;
            }
            printf("\nSending message... %s\n", buffer);
        }
    } else {
        while (1) {
            if ((n = recvfrom(sockfd, buffer, DIM - 1, 0, (struct sockaddr *) &serveraddr, &len)) < 0) {
                printf("Errore durante la ricezione del token!\n");
                return -1;        
            }

            printf("\nHai ricevuto un messaggio!\n");
            printf("Message: %s\n", buffer);
        }
    }
}
