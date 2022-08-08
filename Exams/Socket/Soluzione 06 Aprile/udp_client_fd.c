/* Alessandro Maggio X81000134 */
/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#include "timer.h"

void send_package(void);
int sockfd, n, recvint, length;
struct sockaddr_in servaddr, cliaddr;
int *buffer;

int main(int argc, char**argv){

	if (argc != 3){
	  printf("Usage:  udp_server_fd <IP address> <FILE>\n");
	  return 1;
	}
	else
	  printf ("Send file %s, to %s\n", argv[2], argv[1]);

	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(3030);

	FILE *r = NULL;

	r = fopen(argv[2], "rb");
	if(r == NULL){
		printf("Error: fopen\n");
		return -1;
	}

	fseek(r, 0, SEEK_END);
	length = ftell(r);
	fseek(r, 0, SEEK_SET);

	buffer = malloc(length);
	fread(buffer, length, 1,  r);

	fclose(r);
	
	if(start_timer(1000, &send_package)){
		printf("\nTimer error\n");
		return(1);
	}

	while(1){
		if(n != -1 && recvint == length){
			printf("\nFile sent\n");
			break;
		}
	}

	stop_timer();

	return 0;
}

void send_package(void){
   sendto(sockfd, buffer, length, 0, (struct sockaddr *)&servaddr, sizeof(servaddr) );
   n = recvfrom(sockfd, &recvint, sizeof(int), 0, NULL, NULL);
}

