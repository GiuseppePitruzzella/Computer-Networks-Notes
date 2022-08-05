/* UDP Sender */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#define DIM 1000

struct msg {
    int year;
    int month;
    int day;
    int hour;
} msg;

int main(int argc, char* argv[]) { 
  // Socket descriptor
  int sockfd;
  // Init dest IP address
  struct sockaddr_in dest_addr;
  // Timestamp var
  struct msg message;
  time_t t = time(NULL);
  struct tm tm;
  
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
  
  while (1) { 
    sleep(1);
    // Setup timestamp
    tm = *localtime(&t);
    message.year = tm.tm_year + 1900;
    message.day = tm.tm_mday;
    message.month = tm.tm_mon + 1;
    message.day = tm.tm_hour;

    // Send part
    printf("Sending: [%i//%i-%i-%i] \n", message.hour, message.day, message.month, message.year);
    sendto(sockfd, &message, sizeof(message), 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr));
  }
}