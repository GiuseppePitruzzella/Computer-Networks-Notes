#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_USER 10
#define DIM 1024
#define LISTENING_PORT 60000
#define REMOTE_PORT 60001

int manage_msg(int newsockfd,  FILE* listaUtenti,  const char IP[DIM]) {
    int sockfd, n;
    char msg[DIM], buffer2[DIM], line[DIM];
    struct sockaddr_in remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    while(1) {
        printf("[+] In attesa di messaggi da parte del client...\n");
        if ((n = recv(newsockfd, msg, DIM-1, 0)) < 0) {
            perror("recv");
            exit(0);
        }
        printf("[+] Hai ricevuto un nuovo messaggio!\n");
        msg[strcspn(msg,  "\n")] = 0; // Set null terminator 
        printf("> %s\n",  msg);
        fclose(listaUtenti); // Chiudo il file per la registrazione degli utenti
        
        printf("[+] Controllo il tipo di messaggio arrivato...\n");

        // If logout msg
        if (strstr(msg, "logout") != NULL) { //se arriva un messaggio di logout avvio la procedura per chiudere la connessione col client richiedente
            printf("[+] Chiudo la connessione per l'utente...\n");
            close(newsockfd);

            printf("[+] Apro il file per gli utenti...\n");
            printf("[+] Rimuovo l'indirizzo dell'utente dal file...\n");
            FILE* temp = fopen("temp.txt", "w+"); // Create and open temp file
            listaUtenti = fopen("listaUtenti.txt",  "r"); // Open user list file in read-only

            // Leggo il file per gli utenti
            while(fgets(line, DIM, listaUtenti) != NULL) {
                if (strstr(line, IP) == NULL) { // strstr return NULL if line does not contain IP
                    fputs(line, temp);
                    fflush(temp);
                }
            }
            fclose(listaUtenti);
            fclose(temp);

            listaUtenti = fopen("listaUtenti.txt",  "w+"); // Open user list file in write mode
            temp = fopen("temp.txt", "r"); // Open a temp file in read-only
            // Copy user list inside temp file
            while (fgets(line, DIM, temp) != NULL) {
                fputs(line, listaUtenti);
                fflush(listaUtenti);
            }
            fclose(temp); // Close temp file
            fclose(listaUtenti); // Close user list file
            return 0;
        }

        // If just text
        listaUtenti = fopen("listaUtenti.txt", "r+"); // Open user list file in read-only
        n = 0;

        // Loop for each user in list
        while(fgets(line, DIM,  listaUtenti) != NULL) {
            line[strcspn(line,  "\n")] = 0; // Set null terminator
            // Send "who" to sender
            if (strstr(msg, "who") != NULL) {
                strncpy(buffer2 + n, line, DIM - n);
                n = strlen(buffer2);
                strcat(buffer2, " - ");
                n += 3;
            } else { // Forward message to other clients
                if (strcmp(IP, line) != 0) { // Do not forward to sender
                    // Open socket
                    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                        perror("socket");
                        exit(0);
                    }
                    // Set remote address
                    memset(&remote_addr, 0, len);
                    remote_addr.sin_port = htons(REMOTE_PORT);
                    remote_addr.sin_family = AF_INET;
                    inet_pton(AF_INET, line, &remote_addr.sin_addr); // Set address equals to line

                    if (connect(sockfd, (struct sockaddr *) &remote_addr, sizeof(remote_addr)) < 0) { // Connect to client
                        perror("Connect server/client");
                        exit(0);
                    }

                    if (send(sockfd, msg, DIM, 0) < 0) { // Forward message
                        perror("send");
                        exit(0);
                    }

                    close(sockfd);
                }
             }
        }

        if (strstr(msg, "who") != NULL) {
            if (send(newsockfd, buffer2, DIM, 0) < 0) {
                perror("send");
                exit(0);
            }
        }
    }
}

int create_connection(int sockfd) {
    int newsockfd, n;
    struct sockaddr_in remote_addr;
    char msg[DIM];
    socklen_t len = sizeof(struct sockaddr_in);   

    // Creo il file in cui registrare gli utenti
    printf("[+] Creo il file in cui registrare gli utenti...\n");
    FILE* listaUtenti = fopen("listaUtenti.txt", "w+"); 
    fclose(listaUtenti);

    if (listen(sockfd, MAX_USER) < 0) {
        perror("listen");
        exit(0);
    }
    printf("[+] In ascolto per nuove connessioni...\n");
    
    while(1) {
        printf("[+] Creo il file in cui registrare gli utenti...\n");
        listaUtenti = fopen("listaUtenti.txt", "a+");

        memset(&remote_addr, 0, len);
        
        printf("[+] Accetto la nuova connessione...\n");
        newsockfd = accept(sockfd,  (struct sockaddr*) &remote_addr,  &len);
        
        memset(msg, 0, DIM); // Set 0s to buffer
        strncpy(msg, inet_ntoa(remote_addr.sin_addr), DIM); // Copy remote address to msg
        strcat(msg, "\n"); 
        
        printf("[+] Inserisco il nuovo indirizzo all'interno del file...\n");
        if (fputs(msg, listaUtenti) < 0) {
            perror("fputs");
            exit(0);
        }
        fflush(listaUtenti);

        // Check if accept return value is good
        if (newsockfd < 0) {
            perror("accept");
            exit(0);
        }

        
        if (!fork()) {
            close(sockfd); // Close old socket
            msg[strcspn(msg,  "\n")] = 0; // Set null terminator to msg (e.g. "192.168.56.107\n")
            return manage_msg(newsockfd, listaUtenti, strdup(msg));
        } else close(newsockfd); // Close new socket

        printf("[+] Chiudo il file per gli utenti...\n");
        fclose(listaUtenti);
    }
}

int main(int argc, char * argv[]) {
    int sockfd;
    char msg[DIM];
    struct sockaddr_in localaddr;
    socklen_t len = sizeof(struct sockaddr_in);

    printf("[+] Apertura della socket...\n");
    if ((sockfd = socket(AF_INET,  SOCK_STREAM,  0)) < 0) {
        perror("socket()");
        exit(0);
    }

    printf("[+] Setup dell'indirizzo locale...\n");
    memset(&localaddr, 0, len);
    localaddr.sin_port = htons(LISTENING_PORT);
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("[+] Effettuo operazione di bind...\n");
    if (bind(sockfd, (struct sockaddr *) &localaddr, len) < 0) {
        perror("bind");
        exit(0);
    }

    return create_connection(sockfd);   
}