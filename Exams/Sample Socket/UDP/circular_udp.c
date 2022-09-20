// Sistema multi-client UDP in cui i messaggi vengono inoltrati seguendo
// un ordine circolare definito mediante parametro NEXT_HOP.
// Al messaggio inviato al destinatario, segue ACK verso il mittente.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DIM 1024
#define REMOTE_PORT 5550
#define LISTENING_PORT 5550

int main(int argc, char * argv[]) {
    int sockfd, n;
    struct sockaddr_in localaddr, remoteaddr, nexthop;
    socklen_t len = sizeof(struct sockaddr);
    char source[DIM], dest[DIM], text[DIM], msg[DIM];
    char * token;

    if (argc < 3) {
        printf("Error! Use <YOUR_IP> <NEXT_HOP_IP>\n");
        return -1;
    }

    if (!fork()) {
        if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket()");
            exit(1);
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
            perror("setsockopt()");
            exit(1);
        }

        memset(&remoteaddr, 0, len);
        remoteaddr.sin_family = AF_INET;
        remoteaddr.sin_port = htons(REMOTE_PORT);
        inet_pton(AF_INET, argv[2], &(remoteaddr.sin_addr));
        
        while (1) {
            bzero(msg, DIM);
            strcat(msg, argv[1]);
            strcat(msg, ":");
            
            printf("[-] Inserisci il destinatario...\n");
            scanf("%s", &dest);
            // dest[strlen(dest)] = 0;
            printf("[+] Hai inserito l'indirizzo: %s\n", dest);

            strcat(msg, dest);
            strcat(msg, ":");

            printf("[-] Inserisci il messaggio...\n");
            scanf("%s", text);
            // text[strlen(text)] = 0;
            printf("[+] Hai inserito il testo: %s\n", text);

            strcat(msg, text);

            printf("[+] Hai inserito il messaggio: %s\n", msg);

            printf("[+] Invio il messaggio a %s\n", argv[2]);
            if (sendto(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                perror("sendto()");
                exit(1);
            }
        }
    } else {
        if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket()");
            exit(1);
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
            perror("setsockopt()");
            exit(1);
        }

        memset(&localaddr, 0, len);
        localaddr.sin_family = AF_INET;
        localaddr.sin_port = htons(LISTENING_PORT);
        localaddr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sockfd, (struct sockaddr *) &localaddr, len) < 0) {
            perror("bind()");
            exit(1);
        }

        memset(&remoteaddr, 0, len);
        remoteaddr.sin_family = AF_INET;
        remoteaddr.sin_port = htons(REMOTE_PORT);

        memset(&nexthop, 0, len);
        nexthop.sin_family = AF_INET;
        nexthop.sin_port = htons(REMOTE_PORT);
        inet_pton(AF_INET, argv[2], &(nexthop.sin_addr));

        while (1) {
            printf("\n[+] In attesa di messaggi...\n");
            if ((n = recvfrom(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len)) < 0) {
                perror("recvfrom()");
                exit(1);
            }
            msg[n] = 0;
            printf("[+] Hai ricevuto un messaggio!\n> [%s] :: %s\n", inet_ntoa(remoteaddr.sin_addr), msg);

            char tmp[DIM];
            strcpy(tmp, msg);
            token = strtok(tmp, ":"); // Get source address
            char * source = token;
            token = strtok(NULL, ":"); // Get destination address
            char * destination = token;

            printf("[+] Indirizzo sorgente uguale a %s\n", source);
            printf("[+] Indirizzo destinazione uguale a %s\n", destination);

            if (strstr(token, argv[1]) != NULL) { // Address match
                printf("[+] Il messaggio ha raggiunto l'effettivo destinatario!\n");
                if (strstr(msg, "ACK") == NULL) { // Check if ACK
                    bzero(msg, DIM);
                    strcat(msg, destination);
                    strcat(msg, ":");
                    strcat(msg, source);
                    strcat(msg, ":");
                    strcat(msg, "ACK");

                    printf("[+] Inoltro ACK al prossimo nodo, %s...\n", inet_ntoa(nexthop.sin_addr));
                    if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) &nexthop, len) < 0) { // Sending ACK
                        perror("sendto()");
                        exit(1);
                    }
                }
            } else {
                printf("[+] Il messaggio non ha ancora raggiunto l'effettivo destinatario!\n");
                printf("[+] Inoltro il messaggio al prossimo nodo, %s...\n", inet_ntoa(nexthop.sin_addr));
                if (sendto(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &nexthop, len) < 0) { // Forward msg to next hop
                    perror("sendto()");
                    exit(1);
                }                    
            }
        }
    }

    close(sockfd);
    return 0;
}
