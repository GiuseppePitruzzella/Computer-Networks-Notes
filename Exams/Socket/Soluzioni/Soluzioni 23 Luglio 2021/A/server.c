
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#define MAX_LINE 1000
int main(int argc, char**argv)
{ 
    int sockfd, n,sendsockfd;
    extern int errno;
    struct sockaddr_in local_addr, remote_addr;
    socklen_t len = sizeof(struct sockaddr_in );
    char mesg[MAX_LINE];
    char username[MAX_LINE];
    char porta[MAX_LINE];
    char ip[MAX_LINE];
    char file[MAX_LINE];
    char sendline[MAX_LINE];
    char lista[MAX_LINE];
    char ip_mittente[MAX_LINE];
    char mittente[MAX_LINE];
    char ignore[MAX_LINE];
    FILE* login;
    login = fopen("login.txt", "w"); //apro il file dove memorizzo i dati dei client
    fclose(login);

    if(argc < 2)
    { 
        printf("Usa: porta di ascolto del server");
        return 0;
    }	 

    if((sockfd=socket(PF_INET,SOCK_DGRAM,0)) <0)
    { 
        printf("\nError in socket opening ... exit!");
        return -1;
    }

    memset(&local_addr,0,sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port=htons(atoi(argv[1]));
    if(bind(sockfd, (struct sockaddr *) &local_addr, len)<0)
    { 
        printf("\nBinding error! Error code n.%d \n",errno);
        return -1;
    }

    for (;;)
    { 
        n = recvfrom(sockfd,mesg,MAX_LINE-1,0,(struct sockaddr *) &remote_addr,&len);
        mesg[n] = 0;
        printf("From IP:%s Port:%d msg:%s \n", inet_ntoa(remote_addr.sin_addr),  ntohs(remote_addr.sin_port), mesg);
        if(strcmp(mesg,"login")==0){ //se il messaggio è di tipo login devo registrare un client
            n = recvfrom(sockfd,mesg,MAX_LINE-1,0,(struct sockaddr *) &remote_addr,&len);
            mesg[n] = 0;
            //salvare nel file le info del client
            strcpy(username,mesg);
            strcpy(ip,inet_ntoa(remote_addr.sin_addr));
            sprintf(porta, "%d", ntohs(remote_addr.sin_port));
            login = fopen("login.txt", "a"); //apro il file in append
            fputs(username,login);   //nome utente scritto
            fflush(login);
            fputs(ip,login);   //ip scritto
            fflush(login);
            fputs("\n",login);   
            fflush(login);
            fputs(porta,login);   //porta udp scritta
            fflush(login);
            sendto(sockfd,porta,strlen(porta),0,(struct sockaddr *)&remote_addr,len);
            fputs("\n",login);   
            fflush(login);
            memset(mesg,0,strlen(mesg));
            n = recvfrom(sockfd,mesg,MAX_LINE-1,0,(struct sockaddr *) &remote_addr,&len); //PORTA TCP
            mesg[n] = 0;
            fputs(mesg,login);   
            fflush(login);
            n = recvfrom(sockfd,mesg,MAX_LINE-1,0,(struct sockaddr *) &remote_addr,&len); //la lista dei file condivisi
            mesg[n] = 0;
            fputs(mesg,login);   //porta tcp scritta
            fflush(login);
            fputs("X\n",login);
            fflush(login);
            fclose(login);
        }
        else if(strcmp(mesg,"WHO\n")==0 || strcmp(mesg,"WHO")==0){
            login = fopen("login.txt", "r"); //apro il file in lettura
            while (fgets(sendline, MAX_LINE, login)!=NULL) {
                printf("username %s",sendline);
                memset(file,0,strlen(file));
                sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr *)&remote_addr,len); //spedisco l'username
                fgets(sendline, MAX_LINE, login);
                fgets(sendline, MAX_LINE, login);
                fgets(sendline, MAX_LINE, login);
                while(fgets(sendline, MAX_LINE, login)!=NULL){
                    if(strcmp(sendline,"X")==0 || strcmp(sendline,"X\n")==0){
                        break;
                    }
                    strcat(file,sendline);
                }
                printf("Invio la lista dei file collegati\n");
                sendto(sockfd,file,strlen(file),0,(struct sockaddr *)&remote_addr,len);
            }
            char stop[MAX_LINE];
            strcpy(stop,"X"); //invio la fine dei client collegati
            sendto(sockfd,stop,strlen(stop),0,(struct sockaddr *)&remote_addr,len);
            fclose(login);
        }
        else if(mesg[0]=='$'){ //devo cercare il mittente e inviare le informazioni
            memmove(mesg, mesg+1, strlen(mesg));
            printf("Devo cercare il mittente con username: %s",mesg);
            login = fopen("login.txt", "r");
            while (fgets(sendline, MAX_LINE, login)!=NULL) { //ricerca univoca con username
                //sendline[strcspn(sendline, "\n")]=0;
                //printf("Confronto con %s: ",sendline);
                if(strcmp(sendline,mesg)==0){
                    fgets(ip,MAX_LINE,login);
                    sendto(sockfd,ip,strlen(ip),0,(struct sockaddr *)&remote_addr,len); //spedisco ip 
                    fgets(porta,MAX_LINE,login);
                    memset(porta,0,strlen(porta)); 
                    fgets(porta,MAX_LINE,login);
                    sendto(sockfd,porta,strlen(porta),0,(struct sockaddr *)&remote_addr,len); //spedisco porta
                }
            }

        }
        if(fork()==0){ //creo una fork per interrogare periodicamente i client
            int sockfd, n;
            extern int errno;
            struct sockaddr_in local_addr, remote_addr;
            struct timeval read_timeout; //struttura dati che mi permette di ottenere una recvfrom non bloccante
            read_timeout.tv_sec = 0;
            read_timeout.tv_usec = 10;
            socklen_t len = sizeof(struct sockaddr_in );
            char mesg[MAX_LINE];
            FILE* logout; //file di logout utile nel caso in cui un client di scollega
            char portaTCP[MAX_LINE];
            for(;;){
                sleep(60); //ogni 60 secondi invio un messaggio periodico dove se ottengo risposta il client e ancora attivo altrimenti lo cancello dalla lista
                login = fopen ("login.txt", "r");
                logout = fopen ("logout.txt", "w");
                while (fgets(username, MAX_LINE, login)!=NULL && fgets(ip, MAX_LINE, login)!=NULL && fgets(porta, MAX_LINE, login)!=NULL && fgets(portaTCP, MAX_LINE, login)!=NULL) 
                {       
                    ip[strcspn(ip, "\n")]=0;
                    porta[strcspn(porta, "\n")]=0;
                    if((sockfd=socket(AF_INET,SOCK_DGRAM,0)) <0)
                    { 
                        printf("\nError in socket opening ... exit!");
                        return -1;
                    }
                    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout); //imposto la socket in modalità non bloccante
                    memset(&remote_addr,0,len);
                    remote_addr.sin_family = AF_INET;
                    inet_pton(AF_INET, ip, &(remote_addr.sin_addr)); 
                    remote_addr.sin_port=htons(atoi(porta)+1); //porta udp +1
                    //invio del ping
                    printf("Invio il ping a %s sulla porta %s: \n",ip,porta);
                    strcpy(sendline,"TUTTO OK?");
                    sendto(sockfd,sendline,strlen(sendline),0,
                            (struct sockaddr *)&remote_addr, len);
                    n=recvfrom(sockfd,sendline,MAX_LINE-1,0, (struct sockaddr *) &remote_addr, &len);
                    sendline[n]=0;
                    //printf("From IP:%s Port:%d msg:%s \n", inet_ntoa(remote_addr.sin_addr),  ntohs(remote_addr.sin_port), sendline);
                    if(strcmp(inet_ntoa(remote_addr.sin_addr),"0.0.0.0")==0){
                        while(fgets(ignore, MAX_LINE, login)!=NULL){
                            if(strcmp(ignore,"X")==0 || strcmp(ignore,"X\n")==0) break;
                        }
                        continue;
                    } //nel caso in cui non risponde evito di salvarlo nel file
                    if(strcmp(sendline,"TUTTO OK?")==0){
                        while(fgets(ignore, MAX_LINE, login)!=NULL){
                            if(strcmp(ignore,"X")==0 || strcmp(ignore,"X\n")==0) break;
                        }
                        continue;
                    }
                    //aggiornare la lista eventualmente
                    fputs(username,logout);   //nome utente scritto
                    fflush(logout);
                    fputs(ip,logout);   //ip scritto
                    fflush(logout);
                    fputs("\n",logout);   
                    fflush(logout);
                    fputs(porta,logout);   //porta scritta
                    fflush(logout);
                    fputs("\n",logout);   
                    fflush(logout);
                    fputs(portaTCP,logout);   //porta scritta
                    fflush(logout);
                    fputs(sendline,logout);  //scrittura della lista dei file  
                    fflush(logout);
                    fputs("X\n",logout);  //scrittura del carattere di terminazione 
                    fflush(logout);
                    while(fgets(ignore, MAX_LINE, login)){
                        if(strcmp(ignore,"X")==0 || strcmp(ignore,"X\n")==0) break;
                    }
                }
                //sincronizzazione con il file login
                fclose(logout);
                fclose(login);
                logout = fopen("logout.txt", "r");
                login = fopen("login.txt", "w");
                while (fgets(ignore, MAX_LINE, logout)!=NULL) {
                    fputs(ignore, login);
                    fflush(login);
                }
                fclose(logout);
                fclose(login);
            }
        }
    }
}