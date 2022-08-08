#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#define SERV_TCP_PORT 6550
#define MAX 1025

str_echo(int sockfd)
{   int n;
    char line[1025];
    for(;;)
    {   if(!(n=read(sockfd,line,MAX)))
            return;
        if(n<0)
        {   printf("\nErrore in str_echo");
            return;
        }
        if(write(sockfd,line,n) != n)
        {   printf("\nErrore in write");
            return;
        }
    }
}

main()
{   int sockfd,newsockfd,child,clilen,j;
    struct sockaddr_in cli_addr, serv_addr;
    extern int errno;
    if((sockfd=socket(AF_INET,SOCK_STREAM,0)) <0)
    {   printf("\nErrore nell'apertura del socket");
        return -1;
    }
    bzero((char *) &serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_TCP_PORT);
    if(bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr))<0)
    {   printf("\nErrore nel binding. Errore %d \n",
               errno);
        return -1;
    }
    listen(sockfd,5);

    for(;;)
    {   clilen=sizeof(cli_addr);
        if((newsockfd=accept(sockfd,(struct sockaddr *)
                             &cli_addr, &clilen))<0)
        {   printf("\nErrore in fase di accept");
            return -1;
        }
        if( (child=fork()) <0)
        {   printf("\nErrore di fork");
            return -1;
        }
        else if(!child)
        {   close(sockfd);
            printf("%d\n", child);
            str_echo(newsockfd);
            exit(0);
        }
        close(newsockfd);
    }
}