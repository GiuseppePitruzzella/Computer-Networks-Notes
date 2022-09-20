#include <sys/types.h>     /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DIM 1024
#define USERNAME_DIM 24
#define REMOTE_PORT 60000
#define LISTENING_PORT 60001

int listener() {
    int sockfd, newsockfd;
    char in_msg[DIM];
    struct sockaddr_in localaddr, remoteaddr;
    socklen_t len = sizeof(struct sockaddr_in);

    printf("[+] Apertura della socket per listener...\n");    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(0);
    }

    printf("[+] Setup dell'indirizzo locale...\n");
    memset(&localaddr, 0, len);
    localaddr.sin_port = htons(LISTENING_PORT);
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = INADDR_ANY;
    
    printf("[+] Effettuo operazione di bind...\n");
    if (bind(sockfd, (struct sockaddr *) &localaddr, len) < 0) {
        perror("bind");
        exit(0);
    }

    printf("[+] In ascolto per nuove connessioni...\n");
    if (listen(sockfd, 1) < 0) {
        perror("listen");
        exit(0);
    }
    
    while(1) {
        printf("[+] Accetto la richiesta di connessione...\n");
        if ((newsockfd = accept(sockfd, (struct sockaddr*) &remoteaddr, &len)) < 0) {
            perror("accept");
            exit(0);
        }
        
        printf("[+] In ascolto per nuovi messaggi...\n");
        if (recv(newsockfd, in_msg, DIM, 0) < 0) {
            perror("recv");
            exit(0);
        }

        printf("[+] Hai ricevuto un nuovo messaggio!\n");
        in_msg[strcspn(in_msg, "\n")] = 0; // Set null terminator
        printf("%s\n", in_msg); // Print msg
        printf("> ");

        close(newsockfd);
    }

    return 0;
}

int chat (const int sockfd, char username[USERNAME_DIM]) {
    int n;
    char in_msg[DIM], out_msg[DIM];

    printf("[+] Setup del messaggio...\n");
    username[strcspn(username, "\n")] = 0; // Set null terminator
    strncpy(in_msg, username, DIM); // Copy username inside in_msg
    n = strlen(in_msg); // Get in_msg length
    strncpy(in_msg + n, ": ", DIM - n); // Copy ":" inside in_msg
    n++; // Shift for ":" char
    printf("> ");

    while (fgets(in_msg + n, DIM - 1 - n, stdin) != NULL) {
        printf("[+] Invio il messaggio al server...\n");
        if (send(sockfd, in_msg, strlen(in_msg), 0) < 0) {
            perror("send");
            exit(0);
        }

        if (strstr(in_msg, "logout") != NULL) { // Se il messaggio descritto è uguale a "logout", allora esci
            printf("[+] Effettuo il logout...\n");
            close(sockfd); // Chiusura della socket
            exit(0);
        } else if (strstr(in_msg, "who") != NULL) { // Se il messaggio descritto è uguale a "logout", allora aspetta un messaggio dal server
            printf("[+] Richiedo la lista degli utenti connessi...\n");
            if (recv(sockfd, out_msg, DIM, 0) < 0) {
                perror("recv");
                exit(0);
            }
            printf("Lista degli utenti:\n%s\n", out_msg);
        }
        printf("> ");
    }
    return 0;
}

int main(int argc, char * argv[]) {
    int sockfd;
    char username[DIM];
    struct sockaddr_in remoteaddr;
    socklen_t len = sizeof(struct sockaddr_in);

    if (argc != 2) {
        perror("Inserire l'ip del server");
        exit(0);
    }

    printf("Inserisci il tuo username: ");
    if (fgets(username, DIM, stdin) == NULL) {
        perror("fgets()");
        exit(0);
    }

    printf("\n[+] Apertura della socket...\n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(0);
    }

    printf("[+] Setup dell'indirizzo remoto...\n");
    memset(&remoteaddr, 0, len);
    remoteaddr.sin_port = htons(REMOTE_PORT);
    remoteaddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &remoteaddr.sin_addr);

    printf("[+] Tentativo di connessione al server...\n");
    connect(sockfd, (struct sockaddr *) &remoteaddr, sizeof(remoteaddr));

    if (fork() == 0) {
        return chat(sockfd, username);
    } else{
        return listener();
    }

}