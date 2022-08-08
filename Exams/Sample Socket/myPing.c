#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char**argv) {
    int sockfd, n;
    struct sockaddr_in remote_addr;
    char sendline[32];
    /* Struttura dati per calcolare il tempo */
    struct timespec Tstart; 

    socklen_t len = sizeof(struct sockaddr_in);

    /* Se i valori forniti in input sono diversi da 3 ritorna un errore*/
    if (argc != 3) { 
        printf("usage:  UDPclient <remote_IP remote_port>\n");
        return 1;
    }
    /* Se il valore ritornato dal SocketFileDescriptor è minore di 0 allora vi è stato un errore in quest'ultima */
    if ((sockfd=socket(AF_INET,SOCK_DGRAM,0)) <0) {
        printf("\nError in socket opening ... exit!");
        return -1;
    }
    /* Se ci troviamo nel processo padre ??? */
    if (fork()==0) {
        memset(&remote_addr,0,len);
        remote_addr.sin_family = AF_INET;
        inet_pton(AF_INET, argv[1], &(remote_addr.sin_addr)); 
        remote_addr.sin_port = htons(atoi(argv[2]));

        for (int n=0; n<10; ++n) {
            /* Richiedo il tempo preciso al momento */
            clock_gettime(CLOCK_REALTIME, &Tstart);
            /* Scrivo il valore n spostandomi di 4 caratteri */
            memcpy(sendline,&n,4);
            memcpy(sendline+4, &Tstart, sizeof(Tstart));
            sendto(sockfd,sendline,20,0, (struct sockaddr *)&remote_addr, len);
            /* Anche se non ci permette di inviare  */
            sleep(1);
            printf("Sending ping n=%d\n",n);  
        }
    } else {
        int n2;
        long sec, nsec;
        struct timespec T1, T2;
        char recvline[32];

        /* Loop infinito */
        for (;;) {
            recvfrom(sockfd,recvline,20,0, (struct sockaddr *) &remote_addr, &len);
            /* Richiedo il tempo preciso al momento per inserirlo nella variabile T2 */
            clock_gettime(CLOCK_REALTIME, &T2);
            memcpy(&n2, recvline,4);
            memcpy(&T1,recvline+4, sizeof(T1));
            /** Differenza per calcolare il tempo trascorso
             *  Si noti che la struttura fornisce secondi e nanosecondi 
             */
            sec = T2.tv_sec - T1.tv_sec; 
            nsec = T2.tv_nsec - T1.tv_nsec; 
            /* Stampo a video i valori temporali */
            printf("Ping from:%s Port:%d Ping:%d Time: sec=%ld nsec=%ld \n", inet_ntoa(remote_addr.sin_addr),  ntohs(remote_addr.sin_port),n2, sec, nsec);
        }
    }
}