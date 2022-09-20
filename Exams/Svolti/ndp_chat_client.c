#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DIM 1024
#define REMOTE_PORT 6669
#define LISTENING_PORT 6669

int main(int argc, char *argv[]) { 
    int sockfd, n, login, i = 0;
    struct sockaddr_in myaddr, serveraddr;
    socklen_t len = sizeof(struct sockaddr_in);
    char msg[DIM]; // Message
    char * addr; // Remote IP Address

    if (argc < 2) {
        printf("Error! Use <SERVER_IP>");
        return 0;
    }

    // Apertura della socket
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) { 
        printf("\nError in socket opening ... exit!");
        return -1;
    }

    // Set dell'indirizzo del server
    memset(&serveraddr, 0, len);
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(serveraddr.sin_addr));
    serveraddr.sin_port = htons(REMOTE_PORT);

    // Set dell'indirizzo locale
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(LISTENING_PORT);

    // Bind
    if (bind(sockfd, (struct sockaddr *) &myaddr, len) < 0) {
        printf("Binding error!");
        return -1;
    }

    while (1) {
        // Login per effettare la registrazione al servizio offerto dal server
        printf("Digita <1> per effettuare il login con il server... ");
        scanf("%d", &login);
        // Se il valore inserito è positivo allora viene inviato il messaggio di login al server.
        if (login) {
            if (sendto(sockfd, &login, sizeof login, 0, (struct sockaddr *) &serveraddr, len) < 0) {
                printf("Errore durante l'invio del messaggio!\n");
                return -1;
            }
        } else {
            return 0;
        }

        for (int i = 0; i < 7; i++) {
            if ((n = recvfrom(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &serveraddr, &len)) < 0) {
                printf("Errore durante la ricezione del messaggio!\n");
                return -1;
            }
            msg[n] = 0;
            printf("%s: %s\n", inet_ntoa(serveraddr.sin_addr), msg);
            sleep(5);
        }

        printf("Session expired!\n");
    }          
}
