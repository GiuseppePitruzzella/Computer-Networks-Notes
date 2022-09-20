/* Alessandro Maggio X81000134 */
/* Sender */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define RECV_PORT 7550
#define RECV_IP "127.0.0.1"

main(){ 

	int sockfd;
	struct sockaddr_in recv_addr;
	extern int errno;
	int i,n;
	char line[100];

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0)) <0){ 
		printf("\nErrore nell'apertura del socket");
		return -1;
	}

	memset( &recv_addr,0, sizeof(recv_addr));
	recv_addr.sin_family = AF_INET;
	recv_addr.sin_addr.s_addr = inet_addr(RECV_IP);
	recv_addr.sin_port = htons(RECV_PORT);

  	time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
	sprintf(line,"%s", buffer);
	sendto(sockfd, line, strlen(line)+1, 0, (struct sockaddr *)&recv_addr, sizeof(recv_addr));
	printf("Messaggio inviato! %s\n", line);

}
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTMzOTI5NDA4OV19
-->