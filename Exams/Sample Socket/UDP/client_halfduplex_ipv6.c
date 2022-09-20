/* Simple IPv6 half-duplex UDP Client */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char**argv) { 
	int sockfd, n; // Socket descriptor and bytes counter
	struct sockaddr_in6 remote_addr; // Remote address
	char sendline[1000]; // Sendto Buffer
	char recvline[1000]; // Recvfrom Buffer
	char ipv6_addr[INET6_ADDRSTRLEN]; // Litteral local address
	socklen_t len = sizeof(struct sockaddr_in6); // IPv6 address length

	// Input parameters check
	if (argc != 3) {
		printf("usage: UDPclient <remote_IP remote_port>\n");
		return 1;
	}

	// Opening socket
	if((sockfd=socket(AF_INET6, SOCK_DGRAM, 0)) <0) { 
		printf("\nError in socket opening ... exit!");
		return -1;
	}

	// Setup remote address
	memset(&remote_addr, 0, len);
	remote_addr.sin6_family = AF_INET6;
	inet_pton(AF_INET6, argv[1], &(remote_addr.sin6_addr)); 
	remote_addr.sin6_port=htons(atoi(argv[2]));

	// Get input
	while (fgets(sendline, 1000, stdin) != NULL) {
		// Send message
		sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&remote_addr, len);
		// Recv something
		n=recvfrom(sockfd, recvline, 999, 0, (struct sockaddr *) &remote_addr, &len);
		// Mark end of received message
		recvline[n]=0;
		// Set again remote address
		inet_ntop(AF_INET6, &(remote_addr.sin6_addr), ipv6_addr, INET6_ADDRSTRLEN);
		// Print received message
		printf("From IPv6:%s Port:%d msg:%s \n", ipv6_addr,  ntohs(remote_addr.sin6_port), recvline);
	}
}
