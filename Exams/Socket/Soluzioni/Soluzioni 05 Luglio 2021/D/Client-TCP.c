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

    // Create socket
    if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        perror("Errore creazione socket\n");
        exit(-1);
    }

    // Setup remote address
    // Make sure struct is empty
    memset(&remote_addr, 0, addr_len);
    // Set family to remote address
    remote_addr.sin_family = AF_INET;
    // Set server address to remote address
    remote_addr.sin_addr.s_addr = inet_addr(argv[1]);
    // Set port to remote address
    remote_addr.sin_port = htons(atoi(argv[2]));

    // Connect to remote server socket
    if (connect(sock, (struct sockaddr*) &remote_addr, addr_len) < 0 ){
        printf("Collegamento non riuscito %d\n", errno);
    }

    char buff[DIM_BUFF];
    int logout = 0;
    if(!fork()){
        // Child, role: send msg to server
        while(!logout){
            // Make sure buffer is empty
            memset(buff, 0 ,DIM_BUFF);
            printf("-> ");
            // Get input from stdin
            scanf("%s",buff);
            // Send msg
            send(sock, buff, strlen(buff), 0);
            // If msg equals to logout, stop loop
            if(strcmp(buff, "LOGOUT") == 0)
                logout = 1;
        }
    } else {
        // Parent, role: rcv broadcast msg from server
        while (!logout){
            // Recv part
            recv(sock, buff, DIM_BUFF-1, 0);
            // Print received msg
            printf("%s\n",buff);
            if (strcmp(buff, "LOGOUT") == 0)
                logout = 1;
            memset(buff,0, DIM_BUFF);
        }
	    close(sock);
    }
    exit(0);
}

