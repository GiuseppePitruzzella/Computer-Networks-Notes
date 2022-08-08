/* UDP Receiver - Server*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define DIM 256

int main(int argc, char* argv[]) { 
  // Socket descriptor
  int sockfd;
  // Init of local and remote IP address
  struct sockaddr_in local_addr, remote_addr;
  socklen_t len = sizeof(struct sockaddr_in);

  char buffer[DIM];
  
  // Input params check
  if (argc < 2) {
    printf("Error! Use: receiver listening_PORT");
    return 0;
  }  
  
  // Create socket
  if ((sockfd = socket(PF_INET,SOCK_DGRAM,0)) <0) { 
    printf("Error in socket opening ... exit!");
    return -1;
  }

  // Setup local address
  // memset((char *) &local_addr,0,len);
  memset(&local_addr, 0, len);
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(atoi(argv[1]));

  // Binding part
  if (bind(sockfd, (struct sockaddr *) &local_addr, len) < 0) { 
    printf("Binding error! Error code n.%d \n", errno);
    return -1;
  }
  
  // Recv
  for (;;) { 
    recvfrom(sockfd, buffer, DIM,0, (struct sockaddr *) &remote_addr, &len);
    printf("Packet from IP:%s Port:%d msg:%s \n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), buffer);
  }
}
