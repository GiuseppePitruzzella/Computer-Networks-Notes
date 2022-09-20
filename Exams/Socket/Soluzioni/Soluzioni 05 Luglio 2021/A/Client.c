//Francesco Cerruto 1000005927

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DIM_LINE 1000 

int main(int argc, char* argv[]) {
    int sockfd, newsockfd, n;
    struct sockaddr_in local_addr, remote_addr;
    socklen_t len;
    char sendline[DIM_LINE];
    char receiveline[DIM_LINE];

    if (argc != 4) {
        printf("Error! Use <SERVER_IP> <REMOTE_PORT> <LISTENING_PORT>")
        exit(1);
    }

    if (fork()==0) {
        if ((sockfd=socket(PF_INET, SOCK_STREAM, 0))<0) {
            fprintf(stderr, "Errore socket\n");
            exit(1);
        }

        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(atoi(argv[3]));

        if (bind(sockfd, (struct sockaddr *) &local_addr, sizeof(local_addr))<0) {
            fprintf(stderr, "Errore bind\n");
            exit(1);
        }

        //connessione tcp
        listen(sockfd, 1);
        for (;;) {
            len = sizeof(remote_addr);
            newsockfd = accept(sockfd, (struct sockaddr *) &remote_addr, &len);

            if (fork()==0) {
                close(sockfd);
                n = recv(newsockfd, receiveline, DIM_LINE-1, 0);
                receiveline[n]='\0';
                printf("%s", receiveline);
                exit(0);
            }
            else {
                close(newsockfd);
            }
        }
    }
    else {
        if ((sockfd=socket(PF_INET, SOCK_STREAM, 0))<0) {
            fprintf(stderr, "Errore socket\n");
            exit(1);
        }

        memset(&remote_addr, 0, sizeof(local_addr));
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_addr.s_addr = inet_addr(argv[1]);
        remote_addr.sin_port = htons(atoi(argv[2]));

        connect(sockfd, (struct sockaddr*) &remote_addr, sizeof(remote_addr));

        printf("Inserire nome utente: ");
        fgets(sendline, DIM_LINE, stdin);
        send(sockfd, sendline, strlen(sendline), 0);    //nome utente mandato - contiene \n
        strncpy(sendline, argv[3], DIM_LINE);
        send(sockfd, sendline, strlen(sendline), 0);    //porta utente mandata

        while (fgets(sendline, DIM_LINE, stdin)!=NULL && strstr(sendline, "logout")==NULL) {
            send(sockfd, sendline, strlen(sendline), 0);
            if (strstr(sendline, "who")!=NULL) {
                while ((n=recv(sockfd, receiveline, DIM_LINE-1, 0))) {
                    receiveline[n]='\0';
                    if (strstr(receiveline, "FINE DEGLI UTENTI CONNESSI")!=NULL) {
                        break;
                    }
                    printf("%s", receiveline);
                }
            }
        }
        send(sockfd, sendline, strlen(sendline), 0);
        exit(0);
    }
}