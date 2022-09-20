#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define DIM 1024
#define REMOTE_PORT 6669
#define LISTENING_PORT 6669

int main(int argc, char *argv[]) { 
    int sockfd, n;
    struct sockaddr_in myaddr, serveraddr;
    socklen_t len = sizeof(struct sockaddr_in);
    char msg[DIM], addr[32];

    if ((sockfd = socket(PF_INET,SOCK_DGRAM,0)) < 0) { 
        printf("\nError in socket opening ... exit!");
        return -1;
    }

    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(LISTENING_PORT);

    if (bind(sockfd, (struct sockaddr *) &myaddr, len) < 0) {
        printf("Binding error!");
        return -1;
    }

    while (1) {
        if (recvfrom(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &serveraddr, &len) < 0) { // In ascolto di nuovi messaggi di login
            printf("Errore durante la ricezione del messaggio!\n");
            return -1;
        }
        printf("Il client %s si è appena collegato al server!\n", inet_ntoa(serveraddr.sin_addr), msg);

        // Salvo l'indirizzo remoto che ha appena inviato il messaggio
        strcpy(addr, inet_ntoa(serveraddr.sin_addr));

        // Creo un nuovo processo responsabile dell'invio del tempo per il nuovo client
        if (!fork()) {
            memset(&serveraddr, 0, len);
            serveraddr.sin_family = AF_INET;
            inet_pton(AF_INET, addr, &(serveraddr.sin_addr)); 
            serveraddr.sin_port = htons(REMOTE_PORT);

            for (int i = 0; i < 7; i++) {
                time_t t = time(NULL);
                struct tm *ptr = localtime(&t);
                strcpy(msg, asctime(ptr));

                if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) &serveraddr, len) < 0) {
                    printf("Errore durante l'invio del messaggio!\n");
                    return -1;
                }

                sleep(5);                
            }
            return 0;
        }
    }
    return 0;
}
