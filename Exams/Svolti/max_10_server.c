#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DIM 1024
#define REMOTE_PORT 6666
#define LISTENING_PORT 6666

int main(int argc, char * argv[]) {
    int sockfd, n, index = 1;
    struct sockaddr_in localaddr, remoteaddr;
    char msg[DIM];
    const char IP[DIM];
    socklen_t len = sizeof(struct sockaddr_in);

    printf("\n---------------------\n");
    printf("ESAME 28 Giugno 2016\n");
    printf("---------------------\n");

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

    printf("IP Destinatario: ");
    scanf("%s", &IP);
    printf("\nRiceverai messaggi da %s\n", IP);
    inet_pton(AF_INET, IP, &(remoteaddr.sin_addr)); 

    if (bind(sockfd, (struct sockaddr *) &localaddr, len) < 0) {
        printf("Binding error!");
        return -1;
    }

    while (1) {
        if ((n = recvfrom(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len)) < 0) {
            printf("Error during recvfrom()");
            return -1;
        }
        msg[n] = 0;
        printf("Hai ricevuto un messaggio!\n[%s] : %s\n", inet_ntoa(remoteaddr.sin_addr), msg);

        if (sendto(sockfd, "OK", 4, 0, (struct sockaddr *) &remoteaddr, len) < 0) {
            printf("Error during sendto()\n");
            return -1;
        }
    }
    return 0;
}