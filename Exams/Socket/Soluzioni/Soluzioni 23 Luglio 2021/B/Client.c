#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DIM_LINE 1000

int main(int argc, char* argv[]) {
    int sockfd, n, newsockfd;
    struct sockaddr_in local_addr, remote_addr, list;
    socklen_t len = sizeof(struct sockaddr_in );
    char sendline[DIM_LINE];
    char sharedfiles[DIM_LINE]="";
    char lista[DIM_LINE];
    char *concatenafile;

    if(!fork()){//server TCP
        int sockfdserverTCP;
        if((sockfdserverTCP=socket(AF_INET,SOCK_STREAM,0)) <0){
            printf("\nErrore nell'apertura del socket");
            return -1;
        }

        memset((char *) &local_addr,0,sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        local_addr.sin_port = htons(atoi(argv[3])); //porta TCP

        if(bind(sockfdserverTCP, (struct sockaddr *) &local_addr, sizeof(local_addr))<0){ 
            printf("\nErrore nel binding. Errore %d \n",errno);
            return -1;
        }

        listen(sockfdserverTCP,5);
        for(;;) { 
            len = sizeof(local_addr);
            newsockfd = accept(sockfdserverTCP,(struct sockaddr *)&local_addr, &len);
            printf("E' stata ricevuta una richiesta di file\n");
            if (fork() == 0){
                close(sockfdserverTCP);
                char nomefile[DIM_LINE];
                
                n = recv(newsockfd,nomefile,DIM_LINE-1,0);
                nomefile[n]='\0';
                printf("E' stato richiesto il file %s\n",nomefile);
                FILE *in=fopen(nomefile,"r");
                char receiveline[DIM_LINE];
                while(fgets(receiveline,DIM_LINE,in)!=NULL){
                    send(newsockfd,receiveline,strlen(receiveline),0);
                }
                
                close(newsockfd);
                exit(0);      
            }
            else
                close(newsockfd);
        }
        exit(0);
    }

    if(argc < 4){ 
        printf("IP_destinazione Porta_server PortaTCP Porta_client");
	    return 0;
    }

    if((newsockfd=socket(PF_INET,SOCK_DGRAM,0)) <0){ 
        printf("\nError in socket opening ... exit!");
        return -1;
    }

    memset(&local_addr,0,sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port=htons(atoi(argv[4])); //porta di ascolto UDP
    if( bind(newsockfd, (struct sockaddr *) &local_addr, len)<0){ 
        printf("\nBinding error! Error code n.%d \n",errno);
        return -1;
    }

    if((sockfd=socket(PF_INET,SOCK_DGRAM,0)) <0){ 
        printf("\nError in socket opening ... exit!");
        return -1;
    }
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port=htons(atoi(argv[2]));
    remote_addr.sin_addr.s_addr = inet_addr(argv[1]);
    char signal[DIM_LINE]="login";
    sendto(sockfd,signal,strlen(signal), 0, (struct sockaddr *)&remote_addr, len);

    printf("Inserisci username:\n");
    fgets(sendline,DIM_LINE,stdin);
    sendto(sockfd,sendline,strlen(sendline), 0, (struct sockaddr *)&remote_addr, len);

    printf("\nElenca i file che vuoi condividere, fine se hai finito di elencarli\n");
    for(;;) {
        fgets(sendline,DIM_LINE,stdin);
        if(strcmp(sendline,"fine")==0 || strcmp(sendline,"fine\n")==0 ) break; //condizione di arresto dell'elencazione dei file
        sendline[strcspn(sendline,"\n")]=' ';
        concatenafile=strcat(sharedfiles,sendline); //funzione per la concatenazione di stringhe
    }

    printf("\nHai condiviso questi file: %s\n",concatenafile);
    sendto(sockfd,concatenafile,strlen(sharedfiles), 0, (struct sockaddr *)&remote_addr, len);
    //invio la porta di ascolto TCP
    strncpy(sendline, argv[3], DIM_LINE);
    sendto(sockfd,sendline,strlen(sharedfiles), 0, (struct sockaddr *)&remote_addr, len);
    //invio la porta di ascolto UDP
    strncpy(sendline, argv[4], DIM_LINE);
    sendto(sockfd,sendline,strlen(sharedfiles), 0, (struct sockaddr *)&remote_addr, len);

    for(;;){
        char comando1[DIM_LINE]; 
        //l'utente digita uno dei 3 comandi disponibili
        printf("\nPuoi digitare uno dei seguenti comandi: \n\n"); 
        printf("1) lista: per vedere quali sono gli utenti registrati al servizio\n");
        printf("2) username: per conoscere la lista dei file da lui condivisi\n");
        printf("3) logout: per effettuare un logout\n");
        fgets(comando1,DIM_LINE,stdin);
        sendto(sockfd,comando1,strlen(comando1), 0, (struct sockaddr *)&remote_addr, len);

        if(strcmp(comando1,"lista")==0 || strcmp(comando1,"lista\n")==0){ //l'utente vuole sapere quali sono gli utenti registrati
            
            n = recvfrom(newsockfd,lista,DIM_LINE-1,0,(struct sockaddr *) &list,&len);
            lista[n] = '\0';

            printf("Gli utenti registrati sono:\n\n%s\n",lista);
        }
        else if(strcmp(comando1,"logout")==0 || strcmp(comando1,"logout\n")==0){ //richiesto il logout
            close(sockfd);
            close(newsockfd);
            exit(0);
        }
        else{//questo else scatta quando voglio conoscere la lista dei file condivisi da un determinato utente
            char portaRemota[DIM_LINE];
            char ipremoto[DIM_LINE];
            n = recvfrom(newsockfd,lista,DIM_LINE-1,0,(struct sockaddr *) &list,&len);
            lista[n] = '\0';
            n = recvfrom(newsockfd,ipremoto,DIM_LINE-1,0,(struct sockaddr *) &list,&len);
            ipremoto[n] = '\0';
            ipremoto[strcspn(ipremoto,"\n")]='\0';
            n = recvfrom(newsockfd,portaRemota,DIM_LINE-1,0,(struct sockaddr *) &list,&len);
            portaRemota[n] = '\0';
            portaRemota[strcspn(portaRemota,"\n")]='\0';
            printf("\nHa da condividere questi file: %s\n",lista);
            
            if(strcmp(lista,"nessun utente trovato")!=0){
                printf("Vuoi scaricare un file da questo host?\n");
                printf("1) si\n");
                printf("2) no\n");
                char comando2[DIM_LINE];
                fgets(comando2,DIM_LINE,stdin);
                if(strcmp(comando2,"si")==0 || strcmp(comando2,"si\n")==0){
                    printf("\nScrivere il nome del file da scaricare\n");
                    char nomefile[DIM_LINE];
                    fgets(nomefile,DIM_LINE,stdin);
                    nomefile[strcspn(nomefile,"\n")]='\0';
                    while(strstr(lista,nomefile)==NULL){
                        printf("Il file non è disponibile. Devi sceglierne un altro\n");
                        fgets(nomefile,DIM_LINE,stdin);
                        nomefile[strcspn(nomefile,"\n")]='\0';
                    }
                    
                    if(!fork()){
                        FILE *nuovofile=fopen(nomefile,"w");

                        struct sockaddr_in dest_addr;
                        int sockfdclientTCP=socket(AF_INET,SOCK_STREAM,0);

                        memset(&dest_addr, 0, sizeof(dest_addr));
                        dest_addr.sin_family=AF_INET;
                        inet_pton(AF_INET, ipremoto, &(dest_addr.sin_addr)); 
                        dest_addr.sin_port=htons(atoi(portaRemota));
                        printf("\nFrom IP: %s Port: %d  \n", inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port));
                        connect(sockfdclientTCP, (struct sockaddr *) &dest_addr, sizeof(dest_addr));
                        send(sockfdclientTCP,nomefile,strlen(nomefile),0);
                        char receiveline[DIM_LINE];
                        
                        for(;;){
                            n=recv(sockfdclientTCP,receiveline,DIM_LINE-1,0);
                            if(n<=0) break;
                            receiveline[n]='\0';
                            fputs(receiveline,nuovofile);
                            fflush(nuovofile);
                        }
                        fclose(nuovofile);
                        close(sockfd);
                        exit(0);
                    }  
                }
                else{}
            }
        }
    }
}