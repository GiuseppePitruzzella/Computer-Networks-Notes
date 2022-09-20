// TODO: Generate token
// TODO: Invio a tutti i client registrati!!!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>

#define DIM 1024
#define REMOTE_PORT 5552
#define LISTENING_PORT 5552

int main(int argc, char *argv[]) { 
    int fd, sockfd;
    struct sockaddr_in myaddr, remoteaddr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[DIM], addr[32], username[32];
    FILE *users;

    // Creazione del file in cui salvare gli utenti registrati
    if ((users = fopen("users.txt", "a+")) == NULL) {
        printf("Errore durante la creazione del file!\n");
        return -1;
    }

    // Apertura della socket
    if ((sockfd = socket(PF_INET,SOCK_DGRAM,0)) < 0) { 
        printf("\nError in socket opening ... exit!");
        return -1;
    }

    // Setup dell'indirizzo locale
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(LISTENING_PORT);

    // Binding
    if (bind(sockfd, (struct sockaddr *) &myaddr, len) < 0) {
        printf("Binding error!");
        return -1;
    }

    int counter = 0;
    while (1) {   
        bzero(buffer, DIM);
        printf("(1)\tWaiting for sign-up messages...\n");
        if (recvfrom(sockfd, buffer, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len) < 0) { // In ascolto di nuovi messaggi di login
            printf("Error during receive\n");
            return -1;
        }
        printf("(2)\tSign-up: %s\n", inet_ntoa(remoteaddr.sin_addr));

        strcpy(addr, inet_ntoa(remoteaddr.sin_addr));

        // Get username from message
        int n = 0, already_exists = 0;
        for (; buffer[n] != ' '; n++) {} // Find last char of username until space
        strncpy(username, buffer, n);
        username[n] = '\0';
        printf("(3)\tSearching for username collision...\n");
        char line[DIM];  
        if ((users = fopen("users.txt", "r+")) == NULL) {
            printf("Errore durante la creazione del file!\n");
            return -1;
        }
        while (fgets(line, DIM - 1, users)) {
            int i = 0;
            for (i = 0; line[i] != ' '; i++) {} // Find last char of username until space
            line[i] = '\0';
            // Check if equals
            if (!strcmp(line, username)) already_exists = 1;
        }
        

        // Generating token or -1 if username already_exists
        srand(time(0));
        int token = already_exists < 1 ? rand() : -1;

        // Writing into file if 
        if (!already_exists) {
            printf("(4)\tWriting into .txt file...\n");
            fprintf(users, "%s", buffer); // Write message inside users
            fprintf(users, "%c", ' '); // Set space for token
            fprintf(users, "%d", token); // Set token
            fprintf(users, "%c", '\n'); // Set newline
        
            printf("(4.1)\tForcing flush on .txt file...\n");
            fflush(users);
        }

        printf("(5)\tCreating new process for %s...\n", addr);

        if (!fork()) {
            // Apertura della socket per il nuovo client
            if ((sockfd = socket(PF_INET,SOCK_DGRAM,0)) < 0) { 
                printf("\nError in socket opening ... exit!");
                return -1;
            }

            memset(&remoteaddr, 0, len);
            remoteaddr.sin_family = AF_INET;
            inet_pton(AF_INET, addr, &(remoteaddr.sin_addr)); 
            remoteaddr.sin_port = htons(REMOTE_PORT);

            if (!already_exists) {
                printf("(6)\tSending token...\n");
                if (sendto(sockfd, &token, 32, 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                    printf("Error sending message...\n");
                    return -1;
                }
            } else {
                printf("(-1)\tSending error...\n");
                if (sendto(sockfd, &token, 32, 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                    printf("Error sending message...\n");
                    return -1;
                }
                return -1;
            }

            while (1) {
                bzero(buffer, DIM);
                inet_pton(AF_INET, addr, &(remoteaddr.sin_addr));

                printf("\n(7)\tWaiting for messages from %s...\n", addr);
                if (recvfrom(sockfd, buffer, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len) < 0) {
                    printf("Error during receive...\n");
                    return -1;
                }
                printf("%s\n", buffer); // Print message
                printf("(8)\tForward messages in multicast...\n");

                // if ((users = fopen("users.txt", "r+")) == NULL) {
                //     printf("Errore durante la creazione del file!\n");
                //     return -1;
                // }

                rewind(users);
                while (fgets(line, DIM - 1, users)) {
                    char text[DIM];
                    strtok(buffer, " ");
                    strcpy(text, strtok(NULL, " "));

                    int i = 0;
                    for (i = 0; line[i] != ' '; i++) {} // Find last char of username until space
                    line[i] = '\0';
                    // Check if equals
                    if (strcmp(line, addr) != 0) {
                        printf("Forward to %s ...\n", line);
                        bzero(buffer, DIM);
                        strcat(buffer, line);
                        strcat(buffer, " ");
                        strcat(buffer, text);

                        // look at this!!!
                        // Assume clients and server using all same ports
                        inet_pton(AF_INET, line, &(remoteaddr.sin_addr));

                        if (sendto(sockfd, buffer, DIM - 1, 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                            printf("Error sending message...\n");
                            return -1;
                        }
                    }
                }
            }
            return 0;
        }
    }

    fclose(users);
    return 0;
}
