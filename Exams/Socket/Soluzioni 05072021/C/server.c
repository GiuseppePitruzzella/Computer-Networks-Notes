#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char *nome, *indirizzo_ip, *usato;

//file del tipo
//alex,8.8.8.8;1

void leggi_da_file(char* pathname)
{   printf("dentro func\n");
    char* stringa;
    FILE *in;
    char* tmp;

    if ((in = fopen(pathname, "r")) == NULL)
    {
        perror("Errore apertura file\n");
        exit(1);
    }
    getline(NULL, 4, in);
    getline(NULL, 2, in);
    getline(NULL, 15, in);
    getline(NULL, 1, in);

    fclose(in);
}

void scrivi_su_file(char *pathname, char* nome_scrittura, char* indirizzo, char *usato_scr)
{
    FILE *in;

    if ((in = fopen(pathname, "w")) == NULL)
    {
        perror("Errore apertura file 2\n");
        exit(1);
    }


    if (strcmp(usato_scr, "1") == 0)
    {
        fprintf(in,"%s\n",nome_scrittura);
        fprintf(in,"%s\n",indirizzo);
        fprintf(in,"%d\n",1);
    }
    else
    {
        
        
    }

    fclose(in);
}


int main(int argc, char *argv[])
{
    int sockfd, newsockfd, n;
    int username = 0;
    int size = 0;
    struct sockaddr_in local_addr, remote_addr;
    FILE *in;
    socklen_t len;
    char msg[1000];
    
    if (argc != 3)
    {
        perror("Usage: <PORT> <FILE>\n");
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Errore apertura socket\n");
        return -1;
    }

    memset((char *) &local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(atoi(argv[1]));
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0)
    {
        printf("Errore binding, Errore %d \n", errno);
        return -1;
    }

    if (listen(sockfd, 5) == -1)
    {
        perror("Errore su ascolto\n");
        return -1;
    }
    
    printf("Server in ascolto sulla porta %s\n", argv[1]);

    for (;;)
    {
        len = sizeof(remote_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &remote_addr, &len);

        if (fork() == 0)
        {   
            char *scr = "1";
            close(sockfd);

            while(1)
            {
                n = recv(newsockfd, msg, 999, 0);
                strcat(msg, "\0");
                printf("Messaggio ricevuto: %s\n", msg);

                if (username == 0 && n != 0)
                {   
printf("Username\n");
                    scr = "1";
                    scrivi_su_file(argv[2], msg, inet_ntoa(remote_addr.sin_addr), scr);
                    username = 1;
                }
                if (n == 0) //messaggio vuoto
                {   
                    printf("n = 0\n");
                    return 0;
                }
                else if (strcmp(msg, "who") == 0)
                {
                    printf("who\n");
                    //restituisci connessini attive
                    if ((in = fopen(argv[2], "a+")) == NULL)
                    {
                        perror("Errore apertura file su who\n");
                        exit(1);
                    }

                    fseek(in, 0, SEEK_END);
                    size = ftell(in);

                    for (int i = 0; i < size; i++)
                    {
                        leggi_da_file(argv[2]);

                        if(strcmp(usato, "1") == 0)
                        {
                            printf("nome:%s, indirizzo:%s", nome, indirizzo_ip);                       
                        }
                    }

                    fclose(in);
                    
                }
                else if (strcmp(msg, "exit") == 0)
                {    
                    printf("exit\n");
                    leggi_da_file(argv[2]);
                    scr = "0";
                    scrivi_su_file(argv[2], nome, indirizzo_ip, scr);
                }  
                else
                {
                    //mandare messaggio
printf("else\n");
                    if ((in = fopen(argv[2], "a+")) == NULL)
                    {
                        perror("Errore apertura file su who\n");
                        exit(1);
                    }

                    fseek(in, 0, SEEK_END);
                    size = ftell(in);
printf("prima lettura file\n");
                    leggi_da_file(argv[2]);
printf("dopo lettura file\n");
                    for (int i = 0; i < size; i++)
                    {
printf("dentro for %d volte\n", i);
                        if (strcmp(scr, "1") == 0 &&  remote_addr.sin_addr.s_addr != indirizzo_ip)
                        {   
printf("dentro strcmpy\n");
                            if (send(newsockfd, msg, n, 0) == -1)
                            {
                                perror("Errore send\n");
                                exit(1);
                            }
                        }
                    }
                } 
            }
        }
        else
        {
            close(newsockfd);
        }
    }
    return 0;
}
