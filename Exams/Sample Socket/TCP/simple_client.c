/* Sample TCP client */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DIM 1000

int main(int argc, char**argv) { 
  // Socket descriptor and n
  int sockfd, n;
  // Init of local and remote address
  struct sockaddr_in local_addr, dest_addr;
  // Init of send and recv message
  char sendmsg[DIM];
  char recvmsg[DIM];

  // Input params check
  if (argc != 3) { 
    printf("usage: Client IP_address <Port\n");
    return 0;
  }

  // Create socket
  sockfd = socket(AF_INET,SOCK_STREAM,0);

  // Setup address
  // Make sure that the data structure is empty
  memset( &dest_addr, 0, sizeof(dest_addr));
  // Set IPv4 as local address family
  dest_addr.sin_family = AF_INET;
  // Set dest address (i.e. argv[1]) to remote_addr.sin_addr.s_addr
  dest_addr.sin_addr.s_addr = inet_addr(argv[1]);
  // Set listening port to receiver
  // N.B. atoi() converts str to int, htons converts port from host to network byte order
  dest_addr.sin_port = htons(atoi(argv[2]));

  // Connect to destination host
  connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(dest_addr));

  while (fgets(sendmsg, DIM - 1,stdin) != NULL) { 
    // Send msg
    send(sockfd, sendmsg, strlen(sendmsg),0);

    n = recv(sockfd, recvmsg, DIM - 1, 0);
    // Mark the end of message with \0
    recvmsg[n] = 0;
    
    printf("\nPid=%d: received from %s:%d the following: %s\n",getpid(), 
            inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port), recvmsg);
    }
}
