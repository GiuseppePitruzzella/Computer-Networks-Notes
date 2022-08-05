/* UDP Sender */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define DIM 1000

int main(int argc, char* argv[]) { 
  // Socket descriptor
  int sockfd;
  // Init dest IP address
  struct sockaddr_in dest_addr;
  // Message
  char buffer[DIM];
  
  // Input params check
  if (argc < 3) {
    printf("Use: sender IP_dest PORT_dest");
    return 0;
  }  
  
  // Create socket
  if ((sockfd = socket(PF_INET,SOCK_DGRAM,0)) < 0) { 
    printf("\nError in socket opening ... exit!");
    return -1;
  }
  
  // Setup address
  memset( &dest_addr, 0, sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  inet_pton(AF_INET, argv[1], &(dest_addr.sin_addr)); 
  dest_addr.sin_port = htons(atoi(argv[2]));
  
  for (;;) { 
    // Input message
    memset(&buffer, 0, sizeof(buffer));
    printf("\nInsert an message: ");
    scanf("%s", buffer);

    // Send part
    printf("Sending %s\n",buffer);
    sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr));
  }
}