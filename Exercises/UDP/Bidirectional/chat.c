/* Simple IPv4 UDP bidirectional chat */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DIM 1000

int main(int argc, char**argv) { 
    // Socket descriptor
    int sockfd, n;
    // Init IP Address
    struct sockaddr_in local_addr, remote_addr;
    // Set sockaddr struct lenght, useful for bind(), sendto() and rcvfrom()
    socklen_t len = sizeof(struct sockaddr_in);
    // Init msg to send to receiver
    char msg[DIM];
   
   // Input params check
    if (argc < 4) {
        printf("Error! Use <Destination_IP> <Destination_Port> <ListeningPORT>");
        return 0;
    }

    // Create process, one (i.e. child) will have the role of receiver, 
    //      the other (i.e. parent) will have the role of sender

    // Reminder: On success, the PID of the child process is returned in the parent, and 0 is returned in the child.
    if (!fork()) {
        // Child code, receiver role

        // Create socket
        if ((sockfd = socket(PF_INET,SOCK_DGRAM,0)) < 0) { 
            printf("\nError in socket opening ... exit!");
            return -1;
        }

        // Setup local IP address

        // Make sure that the data structure is empty
        memset(&local_addr, 0, sizeof(local_addr));
        // Set IPv4 as local address family
        local_addr.sin_family = AF_INET;
        // Set listening port to receiver
        // N.B. atoi() converts str to int, htons converts port from host to network byte order
        local_addr.sin_port=htons(atoi(argv[3]));

        // Set local addr to socket
        if (bind(sockfd, (struct sockaddr *) &local_addr, len) < 0) {
            printf("Binding error!");
            return -1;
        }

        while (1) {
            // recvfrom() reads incoming data and captures the address from which the data was sent.
            // recvfrom() returns the number of bytes received.
            n = recvfrom(sockfd, msg, DIM - 1, 0, (struct sockaddr *) &remote_addr, &len);
            // Mark the end of message with \0
            msg[n] = 0;
            // ntoa() returns address in dot notation, ntohs() returns port from network to host bye order
            printf("From IP:%s Port:%d msg:%s \n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), msg);
        }
        return 0;

    } else {
        // Parent code, sender role

        // Create socket
        if ((sockfd = socket(PF_INET,SOCK_DGRAM,0)) < 0) { 
            printf("\nError in socket opening ... exit!");
            return -1;
        }

        // Setup remote IP address
        memset(&remote_addr, 0, len);
        // Set IPv4 as remote addr family
        remote_addr.sin_family = AF_INET;
        // Set dest address (i.e. argv[1]) to remote_addr.sin_addr
        inet_pton(AF_INET, argv[1], &(remote_addr.sin_addr)); 
        // Set receiver listening port
        // N.B. atoi() converts str to int, htons converts port from host to network byte order
        remote_addr.sin_port = htons(atoi(argv[2]));

        // Send message
        // Reminder: fgets(msg, len, *fp) read input from file as stdin
        while (fgets(msg, DIM, stdin) != NULL) { 
            sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) &remote_addr, len);
        }
        return 0;
    }
            
    }
