#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAX_LINE 1000 //definizione della massima dimensione
int main(int argc, char**argv)
{ 
    int sockfd, n;
    struct sockaddr_in remote_addr;
    char sendline[MAX_LINE];
    char recvline[MAX_LINE];
    char request[MAX_LINE];
    char ip[MAX_LINE];
    char porta[MAX_LINE];
    char file[MAX_LINE];
    char filedascrivere[MAX_LINE];
    socklen_t len = sizeof(struct sockaddr_in);
    FILE* cond;
    cond = fopen("cond.txt", "w"); //file dove mantengo le informanzioni per le reletive comunicazioni
    fclose(cond);
    FILE* f;
    f = fopen("file.txt", "w"); //file dove mantengo la lista dei file che si vuole condividere
    fclose(f);
    if (argc != 3)
    { 
        printf("usage:  UDPclient <remote_IP remote_port>\n");
        return 1;
    }
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0)) <0)
    { printf("\nError in socket opening ... exit!");
        return -1;
    }
    memset(&remote_addr,0,len);
    remote_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(remote_addr.sin_addr)); 
    remote_addr.sin_port=htons(atoi(argv[2]));
    //fase di registrazione
    printf("Inserisci l'username per registrati al servizio: \n");
    strcpy(sendline,"login");
    sendto(sockfd,sendline,strlen(sendline),  0, (struct sockaddr *)&remote_addr, len); //spedisco la stringa "login" in modo tale da far capire al server che è iniziata una fase di login
    memset(sendline,0,strlen(sendline));
    fgets(sendline, MAX_LINE, stdin);
    sendto(sockfd,sendline,strlen(sendline),  0, (struct sockaddr *)&remote_addr, len); //username inviato
    memset(sendline,0,strlen(sendline));
    n=recvfrom(sockfd,sendline,MAX_LINE-1,0, (struct sockaddr *) &remote_addr, &len); //ricevo la porta di ascolto udp, quella che viene mappata dal SIstema Operativo
    sendline[n]=0;
    //printf("Ricevo la mia porta di ascolto: %s\n", sendline); 
    cond = fopen("cond.txt", "a"); //apro il file di configurazione
    fputs(sendline,cond);   
    fflush(cond); //mappo sul file di configurazione la porta udp
    fputs("\n",cond);
    fflush(cond);
    printf("Inserisci la porta in ascolto TCP: \n");
    fgets(sendline, MAX_LINE, stdin);
    fputs(sendline,cond);   //mappo sul file di configurazione la porta TCP
    fflush(cond);
    fclose(cond);
    sendto(sockfd,sendline,strlen(sendline),  0, (struct sockaddr *)&remote_addr, len); //invio la porta di ascolto TCP
    strcpy(file,"");
    if (fork()==0) //fork sempre in ascolto udp
    {   
        int sockfd, n;
        extern int errno;
        struct sockaddr_in local_addr, remote_addr;
        socklen_t len = sizeof(struct sockaddr_in );
        char mesg[MAX_LINE];
        char file[MAX_LINE];
        if((sockfd=socket(AF_INET,SOCK_DGRAM,0)) <0)
        { 
            printf("\nError in socket opening ... exit!");
            return -1;
        }
        cond = fopen("cond.txt", "r"); //apro il file di configurazione
        char porta[MAX_LINE];
        fgets(porta, MAX_LINE, cond); //prendo la porta di ascolto udp
        fclose(cond);
        porta[strcspn(porta, "\n")]=0;
        //printf("UDP PORTA %s %d",porta,strlen(porta));
        if((sockfd=socket(PF_INET,SOCK_DGRAM,0)) <0)
        { 
            printf("\nError in socket opening ... exit!");
            return -1;
        }
        memset(&local_addr,0,sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_port=htons(atoi(porta)+1); //mi metto in ascolto su una porta diversa
        if(bind(sockfd, (struct sockaddr *) &local_addr, len)<0)
        { 
            printf("\nBinding error! Error code n.%d \n");
            return -1;
        }
        for (;;)
        { 
            n = recvfrom(sockfd,mesg,999,0,(struct sockaddr *) &remote_addr,&len);
            mesg[n] = 0;
            //printf("From IP:%s Port:%d msg:%s \n", inet_ntoa(remote_addr.sin_addr),  ntohs(remote_addr.sin_port), mesg); //ricezione del ping
            f = fopen("file.txt", "r"); // apro il file dove sono contenuti tutti i file da condividere
            memset(file,0,strlen(file));
            memset(sendline,0,strlen(sendline));
            while(fgets(sendline, MAX_LINE,f) != NULL){
                strcat(file,sendline); //creo una stringa con tutti i file da voler condividere
            }
            if(file[strlen(file)]!='\n'){
                file[strlen(file)]='\n';
            }
            sendto(sockfd,file,strlen(file),0,(struct sockaddr *)&remote_addr,len); //invio di ack più eventuale file aggiornato
        }
    }
    if(fork()==0){  // processo sempre in ascolto server TCP 

        int sockfd,newsockfd,n;
        struct sockaddr_in local_addr,remote_addr;
        socklen_t len;
        char mesg[MAX_LINE];
        FILE* sped; //file da speidire
        if((sockfd=socket(AF_INET,SOCK_STREAM,0)) <0)
        { 
            printf("\nErrore nell'apertura del socket");
            return -1;
        }
        memset((char *) &local_addr,0,sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        cond = fopen("cond.txt", "r"); //apro il file e mi metto in ascolto sulla porta TCP
        fgets(porta, MAX_LINE, cond);
        fgets(porta, MAX_LINE, cond);
        fclose(cond);
        //printf("Sono il server TCP e mi metto in ascolto sulla porta: %s",sendline);
        local_addr.sin_port = htons(atoi(sendline));
        
        if(bind(sockfd, (struct sockaddr *) &local_addr, sizeof(local_addr))<0)
        { printf("\nErrore nel binding");
            return -1;
        }
        
        listen(sockfd,5);

        for(;;)
        { len = sizeof(remote_addr);
            newsockfd = accept(sockfd,(struct sockaddr *)&remote_addr, &len);
            
            if (fork() == 0)
            { 
                close(sockfd);
            for(;;)
            { 
                n = recv(newsockfd,mesg,999,0);
                if(n==0) return 0;
                mesg[n] = 0;
                mesg[strcspn(mesg, "\n")]=0;
                printf("Richiesta da %s:%d il file: %s\n",
                    inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), mesg ); //ricevo il file su cui vogliono sapere
                char bufferlettura[MAX_LINE];
                char file[MAX_LINE];
                if((sped = fopen(mesg, "r"))==NULL){ //apro il file che voglio spedire in solo lettura
                    strcpy(file,"Errore nella lettura del file");
                }else{
                    while(fgets(bufferlettura,MAX_LINE,sped)!=NULL){
                        strcat(file,bufferlettura); //lo concateno in un buffer
                    }
                } 
                //printf("buffer: %s",file);
                send(newsockfd,file,strlen(file),0); //invio il contenuto del file
            }   
                return 0;      
            }
            else
                close(newsockfd);
        }
    }else{
        sleep(1);
        printf("Inserisci uno per volta il nome del file altrimenti X: \n");
        f = fopen("file.txt", "a"); //apro in append il file dove memorizzo tutti i file
        while(fgets(sendline, MAX_LINE,stdin) != NULL){
            if(strcmp(sendline,"X")==0 || strcmp(sendline,"X\n")==0){ //nel caso in cui inserisce X significa che non ci sono più file
                break;
            }
            //printf("FILE: %s",sendline);
            //cond = fopen("cond.txt", "a");
            strcpy(filedascrivere,sendline);
            filedascrivere[strcspn(filedascrivere, "\n")]=0;
            fputs(filedascrivere,f);  //salvo nel file i file condivisi senza "/n"
            fflush(f);
            strcat(file,sendline);
            memset(sendline,0,strlen(sendline));
            printf("Inserisci uno per volta il nome del file altrimenti X: \n");
        }
        fclose(f);
        sendto(sockfd,file,strlen(file),  0, (struct sockaddr *)&remote_addr, len); // invio la lista dei file in un unico messaggio
        printf("Invia WHO per sapere la lista dei client connessi: \n");
        memset(sendline,0,strlen(sendline));
        while (fgets(sendline, MAX_LINE,stdin) != NULL)
        { 
            if(strcmp(sendline,"WHO")==0 || strcmp(sendline,"WHO\n")==0){
                sendto(sockfd,sendline,strlen(sendline),0,
            (struct sockaddr *)&remote_addr, len);
                for(;;){
                    n=recvfrom(sockfd,recvline,MAX_LINE-1,0, (struct sockaddr *) &remote_addr, &len);
                    recvline[n]=0;
                    if(strcmp(recvline,"X")==0 || strcmp(recvline,"X\n")==0) break; // se ottengo X significa che sono finiti ed esco
                    printf("Username: %s", recvline); //stampo username e lista di file
                    memset(recvline,0,strlen(recvline));
                    n=recvfrom(sockfd,recvline,MAX_LINE-1,0, (struct sockaddr *) &remote_addr, &len);
                    recvline[n]=0;
                    printf("file: %s", recvline);
                }
            }else{
                //printf("Entro per ottenere informazioni: \n");
                strcpy(request,"$"); //il server capisce che è una richiesta di avere informazioni
                strcat(request,sendline);
                sendto(sockfd,request,strlen(request),0,
            (struct sockaddr *)&remote_addr, len); //invio la richiesta di informazioni con una sintassi del tipo $username
                n=recvfrom(sockfd,ip,MAX_LINE-1,0, (struct sockaddr *) &remote_addr, &len);
                //ip[n]=0;
                ip[strcspn(ip, "\n")]=0;
                printf("Ricevo l'ip %s %d \n", ip, strlen(ip));
                n=recvfrom(sockfd,porta,MAX_LINE-1,0, (struct sockaddr *) &remote_addr, &len);
                //porta[n]=0;
                porta[strcspn(porta, "\n")]=0;
                printf("Ricevo la porta TCP: %s %d \n", porta,strlen(porta));
                //inizializzare la comunicazione TCP
                int sockfd2;
                struct sockaddr_in local_addr2,dest_addr2;
                char sendline2[MAX_LINE];
                char recvline2[MAX_LINE];
                int n2;
                sockfd2=socket(AF_INET,SOCK_STREAM,0);
                memset(&dest_addr2,0,sizeof(dest_addr2));
                dest_addr2.sin_family=AF_INET;
                dest_addr2.sin_addr.s_addr=inet_addr(ip);
                dest_addr2.sin_port=htons(atoi(porta));
                connect(sockfd2,(struct sockaddr*)&dest_addr2,sizeof(dest_addr2));
                char filedaleggere[MAX_LINE];
                char filedacreare[MAX_LINE];
                printf("Inserisci il file da voler leggere\n");
                fgets(filedaleggere,MAX_LINE,stdin);
                //printf("file da leggere: %s\n",filedaleggere);
                send(sockfd2,filedaleggere,strlen(filedaleggere),0);
                strcpy(filedacreare,filedaleggere);
                memset(filedaleggere,0,strlen(filedaleggere));
                n2=recv(sockfd2,filedaleggere,MAX_LINE-1,0);
                filedaleggere[n2]=0;
                printf("Il file ricevuto è : %s\n", filedaleggere); //file ricevuto
                if(strcmp(filedaleggere,"Errore nella lettura del file")!=0){
                    FILE* output=fopen(filedacreare, "w");
                    fputs(filedaleggere,output);
                    fflush(output);
                }
            }
            printf("Vuoi inizializzare la comunicazione con un client? Eventualmente inserisci l'username altrimenti WHO: (aggiornamento della lista su file.txt)\n");
        }
    }
}