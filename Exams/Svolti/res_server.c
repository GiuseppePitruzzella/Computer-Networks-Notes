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

int main(int argc, char * argv[]) {
    int sockfd, n;
    struct sockaddr_in localaddr, remoteaddr;
    FILE * resources;
    char * line = NULL;
    char msg[DIM];
    size_t _len = 0;
    socklen_t len = sizeof(struct sockaddr_in);

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Errore durante l'apertura della socket... ");
        exit(1);
    }

    memset(&localaddr, 0, sizeof localaddr);
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(LISTENING_PORT);
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *) &localaddr, len) < 0) {
        perror("Errore durante l'operazione di binding... ");
        exit(1);
    }

    memset(&remoteaddr, 0, sizeof remoteaddr);
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(REMOTE_PORT);
    // remoteaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    while (1) {
        memset(msg, 0, DIM);
        if ((n = recvfrom(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len)) < 0) {
            printf("Error during recvfrom()");
            return -1;
        }

        msg[n] = 0;
        printf("Hai ricevuto un messaggio!\n");
        printf("[%s] : %s\n", inet_ntoa(remoteaddr.sin_addr), msg);

        if (!strcmp(msg, "list")) {
            if ((resources = fopen("resources.txt", "r+")) == NULL) {
                perror("Errore durante l'apertura del file... ");
                exit(1);
            }

            bzero(msg, DIM);
            rewind(resources);
            
            // Creating message
            strcat(msg, "\n");
            while (getline(&line, &_len, resources) != -1) {
                strcat(msg, "\t");
                strcat(msg, line);
            }
            // Send message to server
            if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                perror("Errore durante l'invio del messaggio... ");
                exit(1);
            }
            printf("Invio la lista di risorse al client!\n\n");

            fclose(resources);
        } else if (!strncmp(msg, "get", 3)) {
            if ((resources = fopen("resources.txt", "r+")) == NULL) {
                perror("Errore durante l'apertura del file... ");
                exit(1);
            }

            // Waiting for resource
            sleep(1);
            memset(msg, 0, DIM);
            if ((n = recvfrom(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len)) < 0) {
                printf("Error during recvfrom()");
                return -1;
            }

            char res[DIM];
            strcpy(res, msg);
            msg[n] = res[n] = 0;

            printf("Hai ricevuto un messaggio!\n");
            printf("[%s] : %s\n", inet_ntoa(remoteaddr.sin_addr), msg);
            
            int exists = 0;
            rewind(resources);
            while (exists != 1 && getline(&line, &_len, resources) != -1) {
                if (!strncmp(line, msg, strlen(msg))) exists = 1;
            }

            if (exists > 0) {
                printf("Risorsa presente all'interno del file!\n");
                char * token = strtok(line, ":");
                token = strtok(NULL, ":"); // Quantity
                int i = atoi(token);

                if (i > 0) {
                    i = i - 1;

                    char new_quantity[2];
                    sprintf(new_quantity, "%d", i);
                    strcat(msg, ":");
                    strcat(msg, new_quantity);

                    strcat(msg, ":");
                    strcat(msg, inet_ntoa(remoteaddr.sin_addr));
                    for (int i = 0; (token = strtok(NULL, ":")) != NULL; i++) {
                        strcat(msg, ":");
                        strcat(msg, token);
                    }
                    printf("Sostituisco con %s\n", msg);

                    FILE * temp = fopen("temp.txt", "a+");
                    rewind(resources);
                    while (getline(&line, &_len, resources) != -1) {
                        if (!strncmp(line, res, strlen(res))) {
                            fputs(msg, temp);
                            fputs("\n", temp);
                        } else {
                            if (strlen(line) > 1) fputs(line, temp);
                        }
                    }
                    fflush(temp);

                    fclose(resources);
                    remove("resources.txt");
                    rename("temp.txt", "resources.txt");

                    if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                        perror("Errore durante l'invio del messaggio... ");
                        exit(1);
                    }
                }
            } else {
                if (sendto(sockfd, "Error 2.2", 10, 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                    perror("Errore durante l'invio del messaggio... ");
                    exit(1);
                }
            }
        } else if (!strcmp(msg, "booked")) {
            if ((resources = fopen("resources.txt", "r+")) == NULL) {
                perror("Errore durante l'apertura del file... ");
                exit(1);
            }

            bzero(msg, DIM);
            rewind(resources);

            // Creating message
            strcat(msg, "\n");
            while (getline(&line, &_len, resources) != -1) {
                char * res, * i;
                res = i = strtok (line, ":");
                i = strtok (NULL, ":");
                if (strtok (NULL, ":") != NULL) {
                    strcat(msg, "\t");
                    strcat(msg, res);
                    strcat(msg, "\n");
                }
            }
            printf("Booked: %s", msg);

            // Send message to server
            if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                perror("Errore durante l'invio del messaggio... ");
                exit(1);
            }
            printf("Invio la lista di risorse al client!\n\n");

            fclose(resources);
        } else if (!strcmp(msg, "free")) {
            if ((resources = fopen("resources.txt", "r+")) == NULL) {
                perror("Errore durante l'apertura del file... ");
                exit(1);
            }

            // Waiting for resource
            sleep(1);
            memset(msg, 0, DIM);
            if ((n = recvfrom(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &remoteaddr, &len)) < 0) {
                printf("Error during recvfrom()");
                return -1;
            }

            char res[DIM];
            strcpy(res, msg);
            msg[n] = res[n] = 0;

            printf("Hai ricevuto un messaggio!\n");
            printf("[%s] : %s\n", inet_ntoa(remoteaddr.sin_addr), msg);

            int exists = 0;
            rewind(resources);
            while (exists != 1 && getline(&line, &_len, resources) != -1) {
                if (!strncmp(line, msg, strlen(msg))) exists = 1;
            }

            if (exists > 0) {
                printf("Risorsa presente all'interno del file!\n");
                char * token = strtok(line, ":");
                token = strtok(NULL, ":"); // Quantity

                int i = atoi(token);
                i = i + 1;

                char new_quantity[2];
                sprintf(new_quantity, "%d", i);
                strcat(msg, ":");
                strcat(msg, new_quantity);

                int found = 0;
                for (int i = 0; (token = strtok(NULL, ":\n")) != NULL; i++) {
                    printf("%d Indirizzo che ha prenotato la risorsa: %s [%s]\n", i, token, inet_ntoa(remoteaddr.sin_addr));

                    if (found != 1 && !strcmp(token, inet_ntoa(remoteaddr.sin_addr))) {
                        found = 1;
                        printf("Rimuovo l'indirizzo che ha prenotato la risorsa...\n");
                    } else {
                        strcat(msg, ":");
                        strcat(msg, token);
                        printf("Aggiungo indirizzo che ha prenotato la risorsa...\n");
                    }
                }

                FILE * temp = fopen("temp.txt", "a+");
                rewind(resources);
                while (getline(&line, &_len, resources) != -1) {
                    if (!strncmp(line, res, strlen(res))) {
                        fputs(msg, temp);
                        fputs("\n", temp);
                    } else {
                        if (strlen(line) > 1) fputs(line, temp);
                    }
                }
                fflush(temp);

                fclose(resources);
                remove("resources.txt");
                rename("temp.txt", "resources.txt");

                if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                    perror("Errore durante l'invio del messaggio... ");
                    exit(1);
                }
            } else {
                if (sendto(sockfd, "Error 2.2", 10, 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                    perror("Errore durante l'invio del messaggio... ");
                    exit(1);
                }
            }
        } else {
            // Comando non riconosciuto
            if (sendto(sockfd, "Error", 6, 0, (struct sockaddr *) &remoteaddr, len) < 0) {
                perror("Errore durante l'invio del messaggio... ");
                exit(1);
            }
        }
    }


}