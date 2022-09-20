#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define DIM 1024
#define REMOTE_PORT 5555
#define LISTENING_PORT 5555

int main(int argc, char * argv[]) {
    int sockfd, n;
    struct sockaddr_in localaddr, remoteaddr;
    socklen_t size = sizeof(struct sockaddr);
    char msg[DIM];
    FILE * newfile;

    if (argc < 2) {
        printf("Error! Use <SERVER_IP>\n");
        return -1;
    }

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("setsockopt()");
        exit(1);
    }

    memset(&remoteaddr, 0, sizeof(remoteaddr));
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(REMOTE_PORT);
    inet_pton(AF_INET, argv[1], &(remoteaddr.sin_addr));

    if (connect(sockfd, (struct sockaddr *) &remoteaddr, sizeof(remoteaddr)) < 0) {
        perror("connect()");
        exit(1);
    }

    while (1) {
        memset(&msg, 0, DIM);
        printf("\nDigita il nome del file che vuoi scaricare...\nFile : ");
        scanf("%s", &msg);

        send(sockfd, msg, strlen(msg), 0);
        printf("[+] Messaggio inviato al server...\n");

        if (strstr(msg, "list")) { // Waiting for the file containing the list of available files
            n = recv(sockfd, msg, DIM - 1, 0);
            msg[n] = 0;
            printf("%s", msg); // Print list         
        } else {
            if ((newfile = fopen(msg, "a")) == NULL) { // Creating new file
                perror("fopen()");
                exit(1);
            }
            while (n = recv(sockfd, msg, DIM - 1, 0)) { // Make a recv until server sends EOF
                if (strstr(msg, "EOF")) // Break if EOF
                    break;
                msg[n] = 0; // Add null terminator char
                fputs(msg, newfile); // Add line to newfile
                printf("%s", msg); // Print received line
            }
            fflush(newfile); // Writing unwritten data to newfile
        }
    }

    close(sockfd);
    return 0;
}