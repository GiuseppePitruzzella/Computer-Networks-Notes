
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>
#define MAX_LINE 1000
#define DAEMON_PORT 60327

int main(int argc, char**argv){ 
    int sockfd, n;
    int sockfd2;
    extern int errno;
    struct sockaddr_in local_addr, remote_addr, daemon_addr;
    socklen_t len = sizeof(struct sockaddr_in );
    char mesg[MAX_LINE];
    char notify[MAX_LINE];
    char nome[MAX_LINE];
    char portaContat[MAX_LINE];
    char ip[MAX_LINE];
    char listaFile[MAX_LINE];
    char file[MAX_LINE];
    char ipA[MAX_LINE];
    char receiveline[MAX_LINE];
    char indirizzoContattare[MAX_LINE];
    char ignore[MAX_LINE];
    char TCPport[MAX_LINE];
    char alert[MAX_LINE];

    FILE* log;
    log = fopen("log.txt", "w");
    fclose(log);
    FILE* addFile;
    addFile = fopen("addFile.txt", "w");        //file per aggiungere i nuovi file alla lista
    fclose(addFile);
   

    if(argc < 5){ 
        printf("Use: Destination_IP Destination_Port Listening_PORT, TCP_port");
	    return 0;
    }	 
    
    //demone per ACK
    if(!fork()){
        char ACK[MAX_LINE];
        if((sockfd2=socket(PF_INET,SOCK_DGRAM,0)) <0){ 
            printf("\nError in socket opening ... exit!");
            return -1;
        }

        memset(&daemon_addr,0,sizeof(daemon_addr));
        daemon_addr.sin_family = AF_INET;
        daemon_addr.sin_port=htons(DAEMON_PORT);

        if(bind(sockfd2, (struct sockaddr *) &daemon_addr, len)<0){ 
            printf("\nBinding error! Error code n.%d \n",errno);
            return -1;
        }

        for(;;){
            n = recvfrom(sockfd2,ACK,MAX_LINE-1,0,(struct sockaddr *) &daemon_addr,&len);  
            ACK[n] = 0;   
            //printf("ho ricevuto un messaggio da ip: %s e porta %d",inet_ntoa(daemon_addr.sin_addr),ntohs(daemon_addr.sin_port));
            addFile = fopen("addFile.txt", "r");
            char buff[MAX_LINE];
            fgets(buff, MAX_LINE, addFile);
            buff[strcspn(buff,"\n")]=0;
            //printf("aggiorno il file con la stringa: %s\n", buff);
            sendto(sockfd2,buff,strlen(buff),0,(struct sockaddr *)&daemon_addr,len);
            memset(buff,0, strlen(buff));
        }
    }   

    //serverTCP

    if(!fork()){
    
        int sockfd,newsockfd,n;
        struct sockaddr_in local_addr,remote_addr;
        socklen_t len;
        char mesg[1000];
        char buffer[MAX_LINE];
        FILE* f;
        if((sockfd=socket(AF_INET,SOCK_STREAM,0)) <0){ 
            printf("\nErrore nell'apertura del socket");
            return -1;
        }
        memset((char *) &local_addr,0,sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        local_addr.sin_port = htons(atoi(argv[4]));
  
        if(bind(sockfd, (struct sockaddr *) &local_addr, sizeof(local_addr))<0){ 
            printf("\nErrore nel binding. Errore %d \n",errno);
            return -1;
        }
  
        listen(sockfd,5);

        for(;;){ 
            len = sizeof(remote_addr);
            newsockfd = accept(sockfd,(struct sockaddr *)&remote_addr, &len);
	  
            if(fork() == 0){       //se sono il figlio chiudo la socket vecchia e lascio aperta quella generata da accept e procedo con la comunicazione su quella
                close(sockfd);
                n = recv(newsockfd,mesg,999,0); //riceverò il nome del file da 
                mesg[n] = 0;
                printf("ho ricevuto una richiesta per il file: %s\n", mesg);
                f = fopen(mesg, "r");                
                fgets(buffer, MAX_LINE, f);  //presuppongo file da una sola riga
                strcat(buffer, " ");
                send(newsockfd,buffer,strlen(buffer),0);   //invierà il file appena bufferizzato
                printf("file %s inviato con successo\n", mesg);
                return 0;      
            }else   close(newsockfd); //il padre chiude la nuova socket e mantenendo aperta quella vecchia si rimette in ascolto
        }
    }
    
    //PARTE UDP 
    if(!fork()){    

        if((sockfd=socket(PF_INET,SOCK_DGRAM,0)) <0){ 
            printf("\nError in socket opening ... exit!");
            return -1;
        }
  
        memset(&remote_addr,0,len);
        remote_addr.sin_family = AF_INET;
        inet_pton(AF_INET, argv[1], &(remote_addr.sin_addr)); 
        remote_addr.sin_port=htons(atoi(argv[2]));
        strcpy(notify, "N");
        sendto(sockfd,notify,strlen(notify),  0, (struct sockaddr *)&remote_addr, len);
        printf("inserisci nome utente\n");
        fgets(mesg, MAX_LINE, stdin);
        sendto(sockfd,mesg,strlen(mesg),  0, (struct sockaddr *)&remote_addr, len); //invio nome utente
        strcpy(mesg,argv[3]);
        sendto(sockfd,mesg,strlen(mesg),  0, (struct sockaddr *)&remote_addr, len); //invio porta
        strcpy(mesg,argv[4]);
        sendto(sockfd,mesg,strlen(mesg),  0, (struct sockaddr *)&remote_addr, len); //invio portaTCP
        printf("inserisci lista dei file \n");
        fgets(file, MAX_LINE, stdin);
        addFile = fopen("addFile.txt", "w");
        file[strcspn(file, "\n")]=0;
        fputs(file, addFile);
        fclose(addFile);
        sendto(sockfd,file,strlen(file),  0, (struct sockaddr *)&remote_addr, len); //invio lista file
 
        printf("Inserisci who o modifica la lista dei file condivisi manipolando il file addFile\n");
        fgets(mesg, MAX_LINE,stdin); 
        sendto(sockfd,mesg,strlen(mesg),  0, (struct sockaddr *)&remote_addr, len); //invio messaggio
 

        close(sockfd);


        // receiver
        if((sockfd=socket(PF_INET,SOCK_DGRAM,0)) <0){ 
            printf("\nError in socket opening ... exit!");
            return -1;
        }
        memset(&local_addr,0,sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_port=htons(atoi(argv[3]));
        if(bind(sockfd, (struct sockaddr *) &local_addr, len)<0){ 
            printf("\nBinding error! Error code n.%d \n",errno);
            return -1;
        }

        for(;;){ 
            
            n = recvfrom(sockfd,alert,MAX_LINE-1,0,(struct sockaddr *) &remote_addr,&len);   //ricevo X o W
            alert[n] = 0;   
           // printf("alert: %s\n", alert);
         
            while(strcmp(nome,"X") != 0 || strcmp(nome,"X\n") != 0)
            {
                n = recvfrom(sockfd,nome,MAX_LINE-1,0,(struct sockaddr *) &remote_addr,&len);   //ricevo il nome dell'utente
                nome[n] = 0;  
                if(strcmp(nome,"X") == 0 || strcmp(nome,"X\n") == 0) break;
                
                n = recvfrom(sockfd,ip,MAX_LINE-1,0,(struct sockaddr *) &remote_addr,&len);   
                ip[n] = 0;                       //ricevo l'indirizzo
                
                n = recvfrom(sockfd,TCPport,MAX_LINE-1,0,(struct sockaddr *) &remote_addr,&len);
                TCPport[n] = 0;                     //ricevo la portaTCP
                
                n = recvfrom(sockfd,file,MAX_LINE-1,0,(struct sockaddr *) &remote_addr,&len);
                file[n] = 0;                //ricevo elenco file
                
                //scrivo su file le credenziali degli utenti
                log = fopen("log.txt", "a");
                fputs(nome, log);
                fflush(log);
                fputs("\n",log);
                fflush(log);
                fputs(ip, log);
                fflush(log);
                fputs("\n", log);
                fflush(log);
                fputs(TCPport, log);
                fflush(log);
                fputs("\n", log);
                fflush(log);
                fputs(file, log);
                fflush(log);
                fputs("\n", log);
                fflush(log);
                fclose(log);
                printf("\n");
                printf("utente: %s, ip: %s, PORTA TCP: %s, \nFILE: %s\n", nome, ip, TCPport, file);
                printf("\n");
                
            }

            char indice[MAX_LINE];
            printf("inserire nome  del client da contattare\n");
            fgets(indice, MAX_LINE, stdin);
            indice[strcspn(indice,"\n")]=0;
            
            log = fopen("log.txt","r");
            while(fgets(nome,MAX_LINE,log) != NULL &&  fgets(ipA,MAX_LINE,log) != NULL && fgets(portaContat,MAX_LINE,log) != NULL &&  fgets(file,MAX_LINE,log) != NULL)
            {
                nome[strcspn(nome, "\n")]=0; 
                ipA[strcspn(ipA, "\n")]=0; 
                portaContat[strcspn(portaContat, "\n")]=0; 
                file[strcspn(file, "\n")]=0; 
                if(strcmp(nome,indice) == 0){
                
                    break;
                }
            }   
            
            
            if(!fork()){
                int sockfd,n;
                struct sockaddr_in local_addr, dest_addr;
                char sendline[1000];
                char recvline[1000];
                sockfd=socket(AF_INET,SOCK_STREAM,0);
                memset( &dest_addr, 0, sizeof(dest_addr));
                dest_addr.sin_family = AF_INET;
                dest_addr.sin_addr.s_addr = inet_addr(ipA);
                dest_addr.sin_port = htons(atoi(portaContat));
                printf("instauro una connessione TCP con indirizzo: %s e porta: %s\n",ipA, portaContat);
                connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(dest_addr));
                printf("inserire il nome del file da scaricare\n");
                fgets(mesg, MAX_LINE,stdin); 
                mesg[strcspn(mesg, "\n")] = 0;
                send(sockfd,mesg,strlen(mesg),0);   //invio il nome del file che voglio
                n=recv(sockfd,recvline,999,0);      //ricevo il file
                recvline[n]=0;
                printf("ho ricevuto il file: %s con contenuto: %s\n", mesg, recvline);
                FILE* new;
                new = fopen(mesg, "w");
                fputs(recvline, new);
                fflush(new);
                fclose(new);
                exit(0); 
            }     
               
        }
        return 0;

    }
   wait(NULL); 
}