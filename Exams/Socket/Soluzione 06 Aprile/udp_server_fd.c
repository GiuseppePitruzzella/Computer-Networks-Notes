
/* Alessandro Maggio X81000134 */   
/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>

int main(int argc, char**argv){
   int sockfd, n;
   struct sockaddr_in servaddr, cliaddr;
   socklen_t len;

   sockfd = socket(AF_INET,SOCK_DGRAM,0);

   memset(&servaddr,0,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port = htons(3030);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

   int buffer[100000];
   FILE *w = NULL;

   for (;;) {
      n = recvfrom(sockfd, buffer, 100000, 0, (struct sockaddr *)&cliaddr, &len);
 
      //strcat(copyFileName, fileName);

      w = fopen("file_copied", "wb");
      if(w == NULL){
         printf("Error: fopen\n");
         return -2;
      }

      fwrite(buffer, n, 1, w);
      fclose(w);
      
      sendto(sockfd, &n, sizeof(n)+1, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
   }
   
}
