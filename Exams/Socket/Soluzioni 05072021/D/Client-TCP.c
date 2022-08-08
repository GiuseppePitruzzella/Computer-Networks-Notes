#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DIM_BUFF 1024

int main(int argc, char** argv){
    int sock;
    struct sockaddr_in remote_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        perror("Errore creazione socket\n");
        exit(-1);
    }

    memset(&remote_addr, 0, addr_len);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(argv[1]);
    remote_addr.sin_port = htons(atoi(argv[2]));

    if( connect(sock, (struct sockaddr*) &remote_addr, addr_len) < 0 ){
        printf("Collegamento non riuscito %d\n", errno);
    }
    char buff[DIM_BUFF];
    pid_t pid = fork();
    int logout = 0;
    if(!pid){
        while(!logout){
            memset(buff, 0 ,DIM_BUFF);
            printf("-> ");
            scanf("%s",buff);
            send(sock, buff, strlen(buff), 0);
            if(strcmp(buff, "LOGOUT") == 0)
                logout = 1;
        }
    }
    else{
        while(!logout){
            recv(sock, buff, DIM_BUFF-1, 0);
            printf("%s\n",buff);
            if( strcmp(buff, "LOGOUT") == 0)
                logout = 1;
            memset(buff,0, DIM_BUFF);
        }
	    close(sock);
    }
    exit(0);
}

