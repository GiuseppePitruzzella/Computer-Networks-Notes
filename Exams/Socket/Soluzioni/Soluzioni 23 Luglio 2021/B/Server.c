#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>

#define DIM_LINE 1000

int main(int argc, char* argv[]) {
    int sockfd, n;
    struct sockaddr_in local_addr, remote_addr;
    socklen_t len = sizeof(struct sockaddr_in );
    char msg[DIM_LINE];
    FILE* login;
    login = fopen("login.txt","w"); //conviene aprirlo in scrittura così da svuotarne l'eventuale contenuto
    fclose(login);

    if(argc < 2) { 
        printf("Porta di ascolto");
	    return 0;
    }

    if((sockfd=socket(PF_INET,SOCK_DGRAM,0)) <0){ 
        printf("\nError in socket opening ... exit!");
        return -1;
    }

    memset(&local_addr,0,sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port=htons(atoi(argv[1]));
    if( bind(sockfd, (struct sockaddr *) &local_addr, len)<0) { 
        printf("\nBinding error! Error code n.%d \n",errno);
        return -1;
    }

    for (;;) {   
        n = recvfrom(sockfd,msg,DIM_LINE-1,0,(struct sockaddr *) &remote_addr,&len);
        msg[n] = '\0';
        msg[strcspn(msg,"\n")]='\0';

        if(strcmp(msg,"login")==0 || strcmp(msg,"login\n")==0) { //fase di registrazione
            n = recvfrom(sockfd,msg,DIM_LINE-1,0,(struct sockaddr *) &remote_addr,&len);//ricevo username
            msg[n] = '\0';
            login = fopen("login.txt","a");
            printf("From IP:%s Port:%d msg:%s \n", inet_ntoa(remote_addr.sin_addr),  ntohs(remote_addr.sin_port), msg);
            //scrivo l'username su file
            fputs(msg,login);
            fflush(login);
            //ricevo e scrivo i file condivisi su file
            n = recvfrom(sockfd,msg,DIM_LINE-1,0,(struct sockaddr *) &remote_addr,&len);
            msg[n] = '\0';
            fputs(msg,login);
            fflush(login);
            fputs("\n",login); 
            fflush(login);
            //scrivo l'IP su file
            fputs(inet_ntoa(remote_addr.sin_addr),login);
            fflush(login);
            fputs("\n",login); 
            fflush(login);
            //ricevo e scrivo la porta TCP su file
            n = recvfrom(sockfd,msg,DIM_LINE-1,0,(struct sockaddr *) &remote_addr,&len);
            msg[n] = '\0';
            fputs(msg,login);
            fflush(login);
            fputs("\n",login); 
            fflush(login);
            //ricevo e scrivo la porta di ascolto del client
            n = recvfrom(sockfd,msg,DIM_LINE-1,0,(struct sockaddr *) &remote_addr,&len);
            msg[n] = '\0';
            fputs(msg,login);
            fflush(login);   
            fputs("\n",login); 
            fflush(login);
            fclose(login);
        }
        else {
            if(!fork()) {
                char ip[DIM_LINE];
                char porta[DIM_LINE];
                char portaTCP[DIM_LINE];
                char username[DIM_LINE];
                char sharedfiles[DIM_LINE];
                char ignore[DIM_LINE];
                if(strcmp(msg,"lista")==0 || strcmp(msg,"lista\n")==0) {
                    login = fopen("login.txt","r");
                    while(fgets(username,DIM_LINE,login)!=NULL  && fgets(ignore,DIM_LINE,login)!=NULL && fgets(ip,DIM_LINE,login)!=NULL
                    && fgets(ignore,DIM_LINE,login)!=NULL && fgets(porta,DIM_LINE,login)!=NULL){
                        ip[strcspn(ip,"\n")]='\0';
                        if(strcmp(ip,inet_ntoa(remote_addr.sin_addr))==0){
                            break;
                        }
                    }
                    fclose(login);
                    
                    int newsockfd;
                    if((newsockfd=socket(PF_INET,SOCK_DGRAM,0)) <0) { 
                        printf("\nError in socket opening ... exit!");
                        return -1;
                    }
                    
                    inet_pton(AF_INET, ip, &(remote_addr.sin_addr)); 
                    remote_addr.sin_port = htons(atoi(porta));
                    printf("\nFrom IP: %s Port: %d msg: %s \n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), msg);

                    char nome[DIM_LINE];
                    char puntodipartenza[DIM_LINE]="";
                    char *lista=NULL;
                    login = fopen("login.txt","r");
                    
                    while(fgets(nome,DIM_LINE,login)!=NULL  && fgets(ignore,DIM_LINE,login)!=NULL && fgets(ignore,DIM_LINE,login)!=NULL
                    && fgets(ignore,DIM_LINE,login)!=NULL && fgets(ignore,DIM_LINE,login)!=NULL){
                        if(strcmp(username,nome)!=0){
                            nome[strcspn(nome,"\n")]=' ';
                            lista=strcat(puntodipartenza,nome);    
                        }  
                    }
                    
                    if(lista!=NULL) printf("Lista di client connessi: %s\n",lista);
                    else lista="Sei l'unico utente ad essere connesso";
                    sendto(newsockfd,lista,strlen(lista),  0, (struct sockaddr *)&remote_addr, len);
                    
                    close(newsockfd);
                    fclose(login);
                    exit(0);
                }
                else if(strcmp(msg,"logout")==0 || strcmp(msg,"logout\n")==0) {//fase di logout
                    login = fopen("login.txt","r");

                    //mi serve un file di appoggio per effettuare il logout
                    FILE *logout=fopen("logout.txt","w");
        
                    while(fgets(username,DIM_LINE,login)!=NULL && fgets(sharedfiles,DIM_LINE,login)!=NULL && fgets(ip,DIM_LINE,login)!=NULL && fgets(portaTCP,DIM_LINE,login)!=NULL
                    && fgets(porta,DIM_LINE,login)!=NULL){
                        ip[strcspn(ip,"\n")]='\0';
                        if(strcmp(ip,inet_ntoa(remote_addr.sin_addr))!=0){ //chi vuole fare logout non viene spostato sul file di appoggio
                            fputs(username,logout);   
                            fflush(logout);
                            fputs(sharedfiles,logout);
                            fflush(logout);
                            fputs(ip,logout);
                            fflush(logout);
                            fputs("\n",logout);
                            fflush(logout);
                            fputs(portaTCP,logout);
                            fflush(logout);
                            fputs(porta,logout);
                            fflush(logout);
                        }
                        else strcpy(ignore,username);
                    }
                    fclose(login);
                    fclose(logout);

                    login = fopen("login.txt","w"); //l'apertura del file in scrittura permette di eliminare l'utente che vuole effettuare il logout
                    logout = fopen("logout.txt","r");
                    
                    //il contenuto viene riportato sul file login.txt
                    while(fgets(username,DIM_LINE,logout)!=NULL  && fgets(sharedfiles,DIM_LINE,logout)!=NULL && fgets(ip,DIM_LINE,logout)!=NULL
                    && fgets(portaTCP,DIM_LINE,logout)!=NULL && fgets(porta,DIM_LINE,logout)!=NULL){
                        fputs(username,login);
                        fflush(login);
                        fputs(sharedfiles,login);
                        fflush(login);
                        fputs(ip,login);
                        fflush(login);
                        fputs(portaTCP,login);
                        fflush(login);
                        fputs(porta,login);
                        fflush(login);
                    }
                    fclose(login);
                    fclose(logout);
                    exit(0);
                }
                else{ //è arrivato un particolare username
                    login = fopen("login.txt","r");
                    char usernamerichiedente[DIM_LINE];
                    char iprichiedente[DIM_LINE];
                    char portarichiedente[DIM_LINE];

                    char iprichiesto[DIM_LINE]="nullo";
                    char filerichiesti[DIM_LINE]="nessun utente trovato";
                    char portaTCPrichiesta[DIM_LINE]="nullo";

                    while(fgets(username,DIM_LINE,login)!=NULL  && fgets(sharedfiles,DIM_LINE,login)!=NULL && fgets(ip,DIM_LINE,login)!=NULL
                    && fgets(portaTCP,DIM_LINE,login)!=NULL && fgets(porta,DIM_LINE,login)!=NULL){
                        username[strcspn(username,"\n")]='\0';
                        ip[strcspn(ip,"\n")]='\0';
                        portaTCP[strcspn(portaTCP,"\n")]='\n';
                        if(strcmp(ip,inet_ntoa(remote_addr.sin_addr))==0){
                            strcpy(usernamerichiedente,username);
                            strcpy(iprichiedente,ip);
                            strcpy(portarichiedente,porta);
                        }
                        if(strcmp(username,msg)==0){
                            strcpy(filerichiesti,sharedfiles);
                            strcpy(iprichiesto,ip);
                            strcpy(portaTCPrichiesta,portaTCP);
                        }
                    }
                    fclose(login);
        
                    printf("Ho ricevuto una richiesta di lista file da %s\n",username);

                    int newsockfd;
                    if((newsockfd=socket(PF_INET,SOCK_DGRAM,0)) <0){ 
                        printf("\nError in socket opening ... exit!");
                        return -1;
                    }
                    
                    inet_pton(AF_INET, iprichiedente, &(remote_addr.sin_addr)); 
                    remote_addr.sin_port = htons(atoi(portarichiedente));
                    printf("From IP:%s Port:%d msg:%s \n", inet_ntoa(remote_addr.sin_addr),  ntohs(remote_addr.sin_port), msg);
                    
                    printf("Lista di file da mandare al client: %s\n",filerichiesti);

                    sendto(newsockfd,filerichiesti,strlen(filerichiesti), 0, (struct sockaddr *)&remote_addr, len);
                    sendto(newsockfd,iprichiesto,strlen(iprichiesto), 0, (struct sockaddr *)&remote_addr, len);
                    sendto(newsockfd,portaTCPrichiesta,strlen(portaTCPrichiesta), 0, (struct sockaddr *)&remote_addr, len);
                    
                    close(newsockfd);

                    exit(0);
                }

            }
        }
    }
}