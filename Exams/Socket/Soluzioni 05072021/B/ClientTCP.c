#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSIZE 1024

#define NOMELEN 24

#define PORTA 60000

#define PORTACLIENT 60001

int listener(){
    int sockfd;
    char buffer[BUFSIZE];
    struct sockaddr_in local_addr, remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        perror("socket");
        exit(0);
    }

    memset(&local_addr,0,len);
    local_addr.sin_port = htons(PORTACLIENT);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd,(struct sockaddr*)&local_addr,len) < 0){
        perror("bind");
        exit(0);
    }

    if(listen(sockfd,1) < 0){
        perror("listen");
        exit(0);
    }
    //printf("In ascolto..\n");
    int newsockfd;
    while(1){
        newsockfd = accept(sockfd, (struct sockaddr*) &remote_addr, &len);
        if(newsockfd < 0){
           perror("accept");
           exit(0);
        }
        //printf("Connesso\n");
        // while(1){
            if(recv(newsockfd,buffer,BUFSIZE,0) < 0){
               perror("recv");
               exit(0);
            }
         // }
        buffer[strcspn(buffer, "\n")] = 0;
        printf("%s\n",buffer);
        printf("> ");
        close(newsockfd);
    }
    
}

int chat(const int sockfd, char nome[NOMELEN]){
    
    char buffer[BUFSIZE];
    char buffer2[BUFSIZE];

    int n;
    nome[strcspn(nome, "\n")] = 0;
    strncpy(buffer,nome,BUFSIZE);
    n = strlen(buffer);
    strncpy(buffer+n,": ",BUFSIZE-n);
    n++;
    printf("> ");
    while (fgets(buffer+n,BUFSIZE-1-n,stdin) != NULL){ 
        if(send(sockfd,buffer,strlen(buffer),0) <0){
            perror("send");
            exit(0);
        }
        if(strstr(buffer,"logout") != NULL){ //se scrive logout chiude la connessione col server ed esce
            close(sockfd);
            exit(0);
        }else if(strstr(buffer,"who") != NULL){ //se scrive who aspetta la lista dei connessi dal server
            if(recv(sockfd,buffer2,BUFSIZE,0) < 0){
                perror("recv");
                exit(0);
            }
            printf("Lista utenti:\n%s\n",buffer2);
        }
        printf("> ");
    }
}

int main(int argc, char **argv){
    int sockfd;
    char nome[BUFSIZE];
    struct sockaddr_in remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    if(argc != 2){
        perror("Inserire l'ip del server");
        exit(0);
    }

    printf("Nome utente: ");

    if(fgets(nome,BUFSIZE,stdin) == NULL){
        perror("Fgets");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        perror("socket");
        exit(0);
    }

    memset(&remote_addr,0,len);
    remote_addr.sin_port = htons(PORTA);
    remote_addr.sin_family = AF_INET;
    inet_pton(AF_INET,argv[1],&remote_addr.sin_addr);

    connect(sockfd, (struct sockaddr *) &remote_addr, sizeof(remote_addr));

    if(fork() == 0){
        return chat(sockfd, nome);
    }else{
        return listener(sockfd);
    }

}