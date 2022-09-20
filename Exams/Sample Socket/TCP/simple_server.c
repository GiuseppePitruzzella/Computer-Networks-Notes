/* Sample TCP server */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#define DIM 1000

int main(int argc, char**argv) { 
  int sockfd,newsockfd,n;
  struct sockaddr_in local_addr,remote_addr;
  socklen_t len;
  char msg[DIM];
  
  // Input params check
  if (argc < 2) {
    printf("Use: server listeing_PORT");
    return 0;
  }	 
  
  // Create socket
  if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    printf("\nErrore nell'apertura del socket");
    return -1;
  }

  // Setup address
  // Make sure that the data structure is empty
  memset((char *) &local_addr, 0, sizeof(local_addr));
  local_addr.sin_family = AF_INET;
  // Accepts connection from any address
  local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  // Set listening port
  local_addr.sin_port = htons(atoi(argv[1]));
  
  // Binding part: set local addr to socket
  if (bind(sockfd, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0) { 
    printf("\nErrore nel binding.");
    return -1;
  }
  
  // Listening to the socket
  listen(sockfd,5);

  // Infinite loop. Server is waiting for connections from clients
  while (1) { 
    // Get len for remote address
    len = sizeof(remote_addr);
    // Get new connection's socket descriptor
    newsockfd = accept(sockfd, (struct sockaddr *)&remote_addr, &len);
	  
    // Reminder: On success, the PID of the child process is returned in the parent, and 0 is returned in the child.
    if (!fork()) { 
      // Child process

      // Close the old socket
      close(sockfd);

      // Waiting for msg from new socket
      while (1) {
        n = recv(newsockfd, msg, DIM - 1, 0);
        // If no bytes received -> return
        if (n == 0)
          return 0;
        // Mark the end of message with \0
        msg[n] = 0;
        // Print
        printf("\nPid=%d: received from %s:%d the following: %s\n",getpid(), 
          inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), msg );
        
        send(newsockfd, msg, n, 0);
      }

      return 0;      

    } else { close(newsockfd); }
  }
}
