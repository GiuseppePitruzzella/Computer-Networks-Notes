#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXUTENTI 10

#define BUFSIZE 1024

#define PORTA 60000

#define PORTACLIENT 60001

int connessioneUtente(int newsockfd, FILE* listaUtenti, const char ipv4[BUFSIZE]){
    char buffer[BUFSIZE];
    char buffer2[BUFSIZE];
    char indirizzi[BUFSIZE];
    int sockfd;
    struct sockaddr_in remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int n;
    while(1){
        n = recv(newsockfd,buffer,BUFSIZE-1,0);
        if(n < 0){
            perror("recv");
            exit(0);
        }
        buffer[strcspn(buffer, "\n")] = 0;
        printf("> %s\n", buffer);
        fclose(listaUtenti);
        
        if(strstr(buffer,"logout") != NULL){ //se arriva un messaggio di logout avvio la procedura per chiudere la connessione col client richiedente
            close(newsockfd);
            FILE* temp = fopen("temp.txt","w+");
            listaUtenti = fopen("listaUtenti.txt", "r");
            while(fgets(indirizzi,BUFSIZE,listaUtenti) != NULL){
                if(strstr(indirizzi,ipv4) == NULL){
                    //indirizzi[strcspn(indirizzi, "\n")] = 0;
                    fputs(indirizzi,temp);
                    fflush(temp);
                }
            }
            fclose(listaUtenti);
            fclose(temp);
            listaUtenti = fopen("listaUtenti.txt", "w+");
            temp = fopen("temp.txt","r");
            while(fgets(indirizzi,BUFSIZE,temp) != NULL){
                //indirizzi[strcspn(indirizzi, "\n")] = 0;
                fputs(indirizzi,listaUtenti);
                fflush(listaUtenti);
            }
            fclose(temp);
            fclose(listaUtenti);
            return 0;
        }

        listaUtenti = fopen("listaUtenti.txt","r+"); //riparto a leggere dall'inizio
        n = 0;
        while(fgets(indirizzi,BUFSIZE, listaUtenti) != NULL){ //creo connessioni tcp e mando il messaggio ai client collegati
            indirizzi[strcspn(indirizzi, "\n")] = 0;
            //printf("Indirizzo letto %s\n",indirizzi);
            if(strstr(buffer,"who") != NULL){ //spedisco who solo a chi richiede 
                strncpy(buffer2+n,indirizzi,BUFSIZE - n);
                n = strlen(buffer2);
                strcat(buffer2," - ");
                n +=3;
            }else{ //altrimenti rispedisco il messaggio a tutti gli utenti
                if(strcmp(ipv4,indirizzi) != 0){ //se uguale non lo spedisco
                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                
                if(sockfd < 0){
                    perror("socket");
                    exit(0);
                 }
                memset(&remote_addr,0,len);
                remote_addr.sin_port = htons(PORTACLIENT);
                remote_addr.sin_family = AF_INET;
                inet_pton(AF_INET,indirizzi,&remote_addr.sin_addr);
                //printf("Prova connessione all'indirizzo %s con la porta %d\n",inet_ntoa(remote_addr.sin_addr),ntohs(remote_addr.sin_port));
                if(connect(sockfd,(struct sockaddr*)&remote_addr,sizeof(remote_addr)) < 0){
                    perror("Connect server/client");
                    exit(0);
                }
                if(send(sockfd,buffer,BUFSIZE,0) < 0){
                    perror("send");
                    exit(0);
                }
                if(sockfd >= 0) close(sockfd);
                }
             }
            }
            if(strstr(buffer,"who") != NULL){
                if(send(newsockfd,buffer2,BUFSIZE,0) < 0){
                    perror("send");
                    exit(0);
                }

        }
    }
}

int creaConnessione(int sockfd){
    FILE* listaUtenti = fopen("listaUtenti.txt","w+");//creo il file
    fclose(listaUtenti);
    int newsockfd;
    struct sockaddr_in remote_addr;
    int n;
    char buffer[BUFSIZE];
    socklen_t len = sizeof(struct sockaddr_in);

    if(listen(sockfd,MAXUTENTI) < 0){
        perror("listen");
        exit(0);
    }
    printf("Server in ascolto..\n\n");
    while(1){
        listaUtenti = fopen("listaUtenti.txt","a+");
        memset(&remote_addr,0,len);
        newsockfd = accept(sockfd, (struct sockaddr*) &remote_addr, &len);
        memset(buffer,0,BUFSIZE);
        strncpy(buffer,inet_ntoa(remote_addr.sin_addr),BUFSIZE);
        strcat(buffer,"\n");
        //printf("Indirizzo ip scritto nel file -> %s\n", buffer);
        if(fputs(buffer,listaUtenti) < 0){
            perror("fputs");
            exit(0);
        }
        fflush(listaUtenti);
        if(newsockfd < 0){
            perror("accept");
            exit(0);
        }
        if(fork() == 0){
            close(sockfd);
            buffer[strcspn(buffer, "\n")] = 0;
            return connessioneUtente(newsockfd,listaUtenti,strdup(buffer));
        }else{
            close(newsockfd);
        }
        fclose(listaUtenti);
    }
}

int main(int argc, char **argv){
    int sockfd;
    char buffer[BUFSIZE];
    struct sockaddr_in local_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        perror("socket");
        exit(0);
    }

    memset(&local_addr,0,len);
    local_addr.sin_port = htons(PORTA);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd,(struct sockaddr*)&local_addr,len) < 0){
        perror("bind");
        exit(0);
    }

    return creaConnessione(sockfd);
    
}