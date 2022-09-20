#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DIM 1024
#define REMOTE_PORT 6543
#define LISTENING_PORT 6543
#define SERVERIP "192.168.56.109"

int main(int argc, char * argv[]) {
    int sockfd, n;
    struct sockaddr_in localaddr, remoteaddr;
    socklen_t len = sizeof(struct sockaddr_in);
    char in_msg[DIM], out_msg[DIM], file[DIM];

    printf("\n------------------------\n");
    printf("ESAME 14 Aprile 2022");
    printf("\n------------------------\n");

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Errore durante l'apertura della socket...");
        exit(1);
    }

    memset(&remoteaddr, 0, sizeof(remoteaddr));
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(REMOTE_PORT);
    inet_pton(AF_INET, SERVERIP, &(remoteaddr.sin_addr));

    // memset(&localaddr, 0, sizeof(localaddr));
    // localaddr.sin_family = AF_INET;
    // localaddr.sin_port = htons(LISTENING_PORT);

    // if (bind(sockfd, (struct sockaddr *) &localaddr, len) < 0) {
    //     perror("Errore durante l'operazione di binding... ");
    //     exit(1);        
    // }

    printf("\n------------------------\n");
    printf("Inserisci i tuoi comandi!");
    printf("\n------------------------\n");

    while (1) {
        memset(in_msg, 0, DIM);
        memset(out_msg, 0, DIM);

        printf("\nComando: ");
        scanf("%s", in_msg);

        if (!strcmp(in_msg, "list")) {
            // Send message to server
            if (sendto(sockfd, in_msg, strlen(in_msg), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                perror("Errore durante l'invio del messaggio... ");
                exit(1);
            }
            // Wait for response
            if (n = recvfrom(sockfd, out_msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len) < 0) {
                perror("Errore durante la ricezione del messaggio... ");
                exit(1);               
            }

            printf("Hai ricevuto un messaggio!\n");
            printf("[%s] : %s\n\n", inet_ntoa(remoteaddr.sin_addr), out_msg);

        } else if (!strcmp(in_msg, "get")) {
            printf("Inserisci il nome della risorsa... \n");
            scanf("%s", file);

            // Send message to server
            if (sendto(sockfd, in_msg, strlen(in_msg), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                perror("Errore durante l'invio del messaggio... ");
                exit(1);
            }
            sleep(1);
            if (sendto(sockfd, file, strlen(file), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                perror("Errore durante l'invio del messaggio... ");
                exit(1);
            }

            // Wait for response
            if (n = recvfrom(sockfd, out_msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len) < 0) {
                perror("Errore durante la ricezione del messaggio... ");
                exit(1);               
            }
            
            printf("Hai ricevuto un messaggio!\n");
            printf("[%s] : %s\n\n", inet_ntoa(remoteaddr.sin_addr), out_msg);
        } else if (!strcmp(in_msg, "booked")) {
            // Send message to server
            if (sendto(sockfd, in_msg, strlen(in_msg), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                perror("Errore durante l'invio del messaggio... ");
                exit(1);
            }
            // Wait for response
            if (n = recvfrom(sockfd, out_msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len) < 0) {
                perror("Errore durante la ricezione del messaggio... ");
                exit(1);               
            }
            in_msg[n] = 0;

            printf("Hai ricevuto un messaggio!\n");
            printf("[%s] : %s\n\n", inet_ntoa(remoteaddr.sin_addr), out_msg);
        } else if (!strcmp(in_msg, "free")) {
            printf("Inserisci il nome della risorsa... \n");
            scanf("%s", file);

            // Send message to server
            if (sendto(sockfd, in_msg, strlen(in_msg), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                perror("Errore durante l'invio del messaggio... ");
                exit(1);
            }
            sleep(1);
            if (sendto(sockfd, file, strlen(file), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                perror("Errore durante l'invio del messaggio... ");
                exit(1);
            }

            // Wait for response
            if (n = recvfrom(sockfd, out_msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len) < 0) {
                perror("Errore durante la ricezione del messaggio... ");
                exit(1);               
            }
            
            printf("Hai ricevuto un messaggio!\n");
            printf("[%s] : %s\n\n", inet_ntoa(remoteaddr.sin_addr), out_msg);
        } else {
            printf("Errore: comando non riconosciuto!\n");
        }
    }
}
