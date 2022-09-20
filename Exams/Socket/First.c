// NOTE: Sebbene il messaggio richiedi fix rispetto
// le condizioni da verificare quando arriva un nuovo messaggio,
// l'esercizio sembra funzionare. I fix si basano sull'evitare che
// siano necessari più giri del dovuto per consegnare un messaggio.
// FIX: Rimuovere forward e far sì che sia il nodo destinatario
// ad evitare la ritrasmissione post confronto tra il suo indirizzo ed dst.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DIM 1024
#define NEXT_HOP "192.168.56.108"
#define NEXT_HOP_PORT 5557
#define PORT 5557

struct message {
    char src[32]; // Real source IP
    char dst[32]; // Real destination IP
    int forward; // If 1, next hop is dst
    int ack; // If 1, last msg has been acked
    char text[DIM]; // Message
};

int main(int argc, char *argv[]) { 
    int sockfd, n;
    struct sockaddr_in myaddr, remoteaddr;
    socklen_t len = sizeof(struct sockaddr_in);
    // char msg[DIM];

    // Create socket
    if ((sockfd = socket(PF_INET,SOCK_DGRAM,0)) < 0) { 
        printf("\nError in socket opening ... exit!");
        return -1;
    }

    if (!fork()) {
        // Child code, receiver role

        // Setup local IP address
        memset(&myaddr, 0, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_port = htons(PORT);

        if (bind(sockfd, (struct sockaddr *) &myaddr, len) < 0) {
            printf("Error using bind()\n");
            return -1;
        }

        while (1) {
            struct message msg;
            if ((n = recvfrom(sockfd, &msg, DIM, 0, (struct sockaddr *) &remoteaddr, &len)) < 0) {
                printf("Error using recvfrom()\n");
                return -1;
            }

            sleep(1);

            printf("SRC: %s \nDST: %s\nFWRD: %d\nACK: %d\n", msg.src, msg.dst, msg.forward, msg.ack);
            printf("MSG: %s\n", msg.text);

            sleep(1);

            if (msg.forward > 0 && msg.ack > 0) {
                printf("Il messaggio di ACK è stato consegnato!\n");
            } else {
                // Setup remote IP address
                memset(&remoteaddr, 0, len);
                remoteaddr.sin_family = AF_INET;
                inet_pton(AF_INET, NEXT_HOP, &(remoteaddr.sin_addr));
                remoteaddr.sin_port = htons(NEXT_HOP_PORT);

                if (msg.forward > 0) {
                    printf("Il messaggio è arrivato a destinazione! Invio ACK...\n");
                    char tmp_src[32];
                    memcpy(tmp_src, msg.src, 32);
                    memcpy(msg.src, msg.dst, 32);
                    memcpy(msg.dst, tmp_src, 32);
                    msg.forward = 0;
                    msg.ack = 1;
                } else {
                    if (!strcmp(NEXT_HOP, msg.dst)) 
                        msg.forward = 1;
                }

                sleep(1);

                printf("Inoltro al nodo successivo...\n");
                if (sendto(sockfd, &msg, DIM, 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                    printf("Error using sendto()!\n");
                    return -1;
                }

                sleep(1);
            }
        }
        return 0;
    } else {
        // Parent code, sender role
        struct message msg;
        // Setup remote IP address
        memset(&remoteaddr, 0, len);
        remoteaddr.sin_family = AF_INET;
        inet_pton(AF_INET, NEXT_HOP, &(remoteaddr.sin_addr));
        remoteaddr.sin_port = htons(NEXT_HOP_PORT);

        printf("Use: <SRC_IP>\n");
        scanf("%s", msg.src);
        printf("Use: <DEST_IP>\n");
        scanf("%s", msg.dst);
        printf("Use: <MSG>\n");
        scanf("%s", msg.text);
        msg.forward = msg.ack = 0;    
        if (sendto(sockfd, &msg, sizeof msg, 0, (struct sockaddr *) &remoteaddr, len) < 0) {
            printf("Error using sendto()!\n");
            return -1;
        }
        printf("Messaggio inviato!\n");

        return 0;
    }
            
}
