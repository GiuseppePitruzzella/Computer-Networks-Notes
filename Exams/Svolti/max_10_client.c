#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DIM 1024
#define REMOTE_PORT 6666
#define LISTENING_PORT 6666

int main(int argc, char * argv[]) {
    int sockfd, n, check = -1;
    struct sockaddr_in localaddr, remoteaddr;
    FILE * dests;
    char * line = NULL;
    char msg[DIM];
    const char IP[DIM];
    size_t _len = 0;
    socklen_t len = sizeof(struct sockaddr_in);

    printf("\n---------------------\n");
    printf("ESAME 28 Giugno 2016\n");
    printf("---------------------\n");

    // Open FILE
    if ((dests = fopen("dest.txt", "r")) == NULL) {
        perror("Error during fopen()...\n");
        exit(1);
    }

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error opening socket()...\n");
        exit(1);
    }

    memset(&remoteaddr, 0, len);
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(REMOTE_PORT);
    memset(&localaddr, 0, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(LISTENING_PORT); 

    while (1) {
        printf("\n---------------------\n");
        printf("Scrivi il tuo messaggio!\n");
        printf("---------------------\n");

        printf("> ");
        scanf("%s", msg);

        printf("\n----------------------------------\n");
        printf("Lista degli indirizzi disponibili!\n");
        printf("----------------------------------\n");
        for (int i = 0; getline(&line, &_len, dests) != -1; i++)
            printf("%d | %s", i + 1, line);
        printf("\n------------------------\n");
        printf("Scegli gli indirizzi...\n");
        printf("------------------------\n");
        printf("Ricorda: per terminare il loop, digita <0.0.0.0>\n");

        for (int i = 0; i < 10; i++) {
            printf("Indirizzo IP: ");
            scanf("%s", &IP);

            if (!strcmp(IP, "0.0.0.0")) break;

            printf("\nInvio il messaggio a %s\n", IP);

            inet_pton(AF_INET, IP, &(remoteaddr.sin_addr));

            if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                printf("Error during sendto()\n");
                return -1;
            }

            if (!fork()) {
                if ((n = recvfrom(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len)) < 0) {
                    printf("Error during recvfrom()");
                    return -1;
                }
                msg[n] = 0;
                printf("Hai ricevuto un messaggio!\n[%s] : %s\n", inet_ntoa(remoteaddr.sin_addr), msg);
            }
        }
    }
    return 0;
}