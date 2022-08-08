#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in local_addr, dest_addr;
    char sendline[1000];
    char recvline[1000];

    if (argc != 3)
    {
        printf("Usage: <IP_server> <PORT>\n");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Errore apertura socket\n");
        exit(1);
    }
    
    // if(bind(sockfd, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0)
    // {
        // printf("Errore binding, Errore %d \n", errno);
        // return -1;
    // }

    memset( &dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(argv[1]);
    dest_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) == -1)
    {
        perror("Errore connessione\n");
        exit(1);
    }
    
    printf("Connessione con server %s stabilita, sulla porta %s\n", argv[1], argv[2]);

    for(;;)
    {
        if (fork() == 0) //mittente
        {   
            //printf("Inserisci un username\n");
            while (fgets(sendline, 999, stdin) != NULL)
            {   
                if (strcmp(sendline, "exit") == 0)
                {
                    send(sockfd, sendline, strlen(sendline), 0);
                    exit(0);
                }
                else
                {
                    send(sockfd, sendline, strlen(sendline), 0);
                }
            }
        }
        else //ricevente
        {
            n = recv(sockfd, recvline, 999, 0);

                if (n != 0)
                {
                    recvline[n] = '\0';

                    printf("Messaggio da %s, contenente %s\n", argv[1], recvline);
                }
        }
    
    }
    return 0;
}
