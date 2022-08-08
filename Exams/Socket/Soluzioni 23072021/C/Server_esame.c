
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#define MAX_LINE 1000
#define DAEMON_PORT 63001

int main(int argc, char**argv){ 
    int sockfd, n;
    
    extern int errno;
    struct sockaddr_in local_addr, remote_addr;
    socklen_t len = sizeof(struct sockaddr_in );
    char mesg[MAX_LINE];
    char ip[MAX_LINE];
    char nome[MAX_LINE];
    char nome_utente[MAX_LINE];
    char porta[MAX_LINE];
    char who[MAX_LINE];
    char ignore[MAX_LINE];
    char ipAppoggio[MAX_LINE];
    char file[MAX_LINE];
    char portaTCP[MAX_LINE];
    char mesgS[MAX_LINE];
    char ack[MAX_LINE];
    char recv[MAX_LINE];
    int portaAppoggio;
    FILE* login;
    FILE* messaggi;
    
    login = fopen("login.txt", "w");
    messaggi = fopen("messaggi.txt", "w");
    fclose(login);
    fclose(messaggi);
    
    
    //DEMONE PER ACK
    if(!fork()){
        int sockfd2;
        struct timeval read_timeout;
        struct sockaddr_in  daemon_addr;
        read_timeout.tv_sec = 0;
        read_timeout.tv_usec = 10;
        socklen_t len = sizeof(struct sockaddr_in );
        FILE* log;
        

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
        setsockopt(sockfd2, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout); //rende sockfd2 non bloccante
        
        for(;;){
            sleep(30);
            log = fopen("login.txt", "r");
            char app[MAX_LINE];
            while(fgets(ignore, MAX_LINE, log) != NULL && fgets(ip, MAX_LINE, log) != NULL && fgets(ignore, MAX_LINE, log) != NULL && fgets(ignore, MAX_LINE, log) != NULL && fgets(ignore, MAX_LINE, log) != NULL)
            {
                strcpy(ack, "V");
                ip[strcspn(ip, "\n")]=0; 
                inet_pton(AF_INET, ip, &(daemon_addr.sin_addr));
                sendto(sockfd2,ack,strlen(ack),  0, (struct sockaddr *)&daemon_addr, len);
    
                sleep(5);
                //ping pong tra stringhe per evitare race condition
                char app[MAX_LINE];
                int x=1;
                if(x == 1){
                    n = recvfrom(sockfd2,recv,999,0,(struct sockaddr *) &daemon_addr,&len);
                    x --;
                    recv[strcspn(recv, "\n")]=0;
                    strcpy(app, recv);
                    memset(recv, 0, strlen(recv));
                }else{
                    n = recvfrom(sockfd2,ack,999,0,(struct sockaddr *) &daemon_addr,&len);
                    x++;
                    ack[strcspn(ack, "\n")]=0;
                    strcpy(app, ack);
                    memset(ack, 0, strlen(ack));
                }
                
                sleep(5);
                printf("n: %d, rec: %s\n", n, app);
                
                if(n <= 0){
                    printf("ack non ricevuto\n");
                    FILE* logout;
                    logout = fopen("logout.txt", "w");
                    login = fopen("login.txt", "r");
                    while(fgets(nome, MAX_LINE, login)!=NULL && fgets(ip, MAX_LINE, login)!=NULL && fgets(porta, MAX_LINE, login)!=NULL && fgets(portaTCP, MAX_LINE, login)!=NULL && fgets(file, MAX_LINE, login)!=NULL)
                    {
                        nome[strcspn(nome,"\n")]=0;
                        ip[strcspn(ip,"\n")]=0;
                        porta[strcspn(porta,"\n")]=0;
                        portaTCP[strcspn(portaTCP,"\n")]=0;
                        file[strcspn(file,"\n")]=0;
                        if(strcmp(inet_ntoa(daemon_addr.sin_addr), ip)!=0) {  
                            fputs(nome, logout);    //inserisco nome 
                            fflush(logout);
                            fputs("\n", logout);
                            fflush(logout);
                            fputs(ip, logout);      //inserisco ip
                            fflush(logout);
                            fputs("\n", logout);
                            fflush(logout);
                            fputs(porta, logout);   //inserisco porta      
                            fflush(logout);
                            fputs("\n", logout);
                            fflush(logout);
                            fputs(portaTCP, logout);   //inserisco portaTCP      
                            fflush(logout);
                            fputs("\n", logout);
                            fflush(logout);
                            fputs(file, logout);   //inserisco file      
                            fflush(logout);
                            fputs("\n", logout);
                            fflush(logout);                    
                        }
                    }
                    fclose(logout);
                    fclose(login);
                    logout = fopen("logout.txt", "r");
                    login = fopen("login.txt", "w");
                    while(fgets(nome, MAX_LINE, logout)!=NULL && fgets(ip, MAX_LINE, logout)!=NULL && fgets(porta, MAX_LINE, logout)!=NULL && fgets(portaTCP, MAX_LINE, logout)!=NULL && fgets(file, MAX_LINE, logout)!=NULL) 
                    {
                        fputs(nome, login);    //inserisco nome 
                        fflush(login);
                        //fputs("\n", login);
                        //fflush(login);
                        fputs(ip, login);      //inserisco ip
                        fflush(login);
                        //fputs("\n", login);
                        //fflush(login);
                        fputs(porta, login);   //inserisco porta      
                        fflush(login);
                        //fputs("\n", login);
                        //fflush(login);
                        fputs(portaTCP, login);   //inserisco portaTCP      
                        fflush(login);
                        //fputs("\n", login);
                        //fflush(login);
                        fputs(file, login);   //inserisco file      
                        fflush(login);
                        fputs("\n", login);
                        fflush(login);  
                    }
                    fclose(logout);
                    fclose(login);
                    
                }else{
                    //dentro ack ho la nuova stringa
                    printf("Daemon: ack arrivato da indirizzo: %s e porta %d\n",inet_ntoa(daemon_addr.sin_addr),  ntohs(daemon_addr.sin_port));
                    login = fopen("login.txt", "r");
                    
                    FILE* temp;
                    temp = fopen("temp.txt", "w");
                    login = fopen("login.txt", "r");
                    while(fgets(nome, MAX_LINE, login)!=NULL && fgets(ip, MAX_LINE, login)!=NULL && fgets(porta, MAX_LINE, login)!=NULL && fgets(portaTCP, MAX_LINE, login)!=NULL && fgets(file, MAX_LINE, login)!=NULL) 
                    {
                        nome[strcspn(nome,"\n")]=0;
                        ip[strcspn(ip,"\n")]=0;
                        porta[strcspn(porta,"\n")]=0;
                        portaTCP[strcspn(portaTCP,"\n")]=0;
                        file[strcspn(file,"\n")]=0;
                        fputs(nome, temp);    //inserisco nome 
                        fflush(temp);
                        fputs("\n", temp);
                        fflush(temp);
                        fputs(ip, temp);      //inserisco ip
                        fflush(temp);
                        fputs("\n", temp);
                        fflush(temp);
                        fputs(porta, temp);   //inserisco porta      
                        fflush(temp);
                        fputs("\n", temp);
                        fflush(temp);
                        fputs(portaTCP, temp);   //inserisco portaTCP      
                        fflush(temp);
                        fputs("\n", temp);
                        fflush(temp);
                        if(strcmp(inet_ntoa(daemon_addr.sin_addr), ip)==0)  fputs(app, temp);  
                        else fputs(file, temp);
                        fflush(temp);
                        fputs("\n", temp);
                        fflush(temp);                    
                    }
                    fclose(temp);
                    fclose(login);
                    temp = fopen("temp.txt", "r");
                    login = fopen("login.txt", "w");
                    while(fgets(nome, MAX_LINE, temp)!=NULL && fgets(ip, MAX_LINE, temp)!=NULL && fgets(porta, MAX_LINE, temp)!=NULL && fgets(portaTCP, MAX_LINE, temp)!=NULL && fgets(file, MAX_LINE, temp)!=NULL) 
                    {
                        fputs(nome, login);    //inserisco nome 
                        fflush(login);
                        //fputs("\n", login);
                        //fflush(login);
                        fputs(ip, login);      //inserisco ip
                        fflush(login);
                        //fputs("\n", login);
                        //fflush(login);
                        fputs(porta, login);   //inserisco porta      
                        fflush(login);
                        //fputs("\n", login);
                        //fflush(login);
                        fputs(portaTCP, login);   //inserisco portaTCP      
                        fflush(login);
                        //fputs("\n", login);
                        //fflush(login);
                        fputs(file, login);   //inserisco file      
                        fflush(login);
                        //fputs("\n", login);
                        //fflush(login);  
                    }
                    fclose(temp);
                    fclose(login);

                    
                }
               
            }
            fclose(log);
        }
    }
    

    //PARTE UDP
    if(!fork()){    
        // receiver
        if((sockfd=socket(PF_INET,SOCK_DGRAM,0)) <0){ 
            printf("\nError in socket opening ... exit!");
            return -1;
        }
        
        memset(&local_addr,0,sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_port=htons(atoi(argv[1]));
        if(bind(sockfd, (struct sockaddr *) &local_addr, len)<0){ 
            printf("\nBinding error! Error code n.%d \n",errno);
            return -1;
        }

        for(;;){     
            //questa recv, o riceve N e porta alla fase di registrazione, oppure who
            n = recvfrom(sockfd,mesg,999,0,(struct sockaddr *) &remote_addr,&len);
            mesg[n] = 0;

            if(strcmp(mesg, "N") == 0 || strcmp(mesg, "N\n") == 0){
                //da eseguire soltanto quando un nuovo client si vuole registrare
                n = recvfrom(sockfd,nome_utente,999,0,(struct sockaddr *) &remote_addr,&len);  //riceve il nome utente
                nome_utente[n] = 0;
                printf("sto registrando l'utente %s\n", nome_utente);
                n = recvfrom(sockfd,porta,999,0,(struct sockaddr *) &remote_addr,&len);  //riceve il numero di porta
                porta[n] = 0;
                n = recvfrom(sockfd,portaTCP,999,0,(struct sockaddr *) &remote_addr,&len);  //riceve il numero di porta
                portaTCP[n] = 0;
                n = recvfrom(sockfd,file,999,0,(struct sockaddr *) &remote_addr,&len);
                file[n]=0;
                
                //scrittura sul file login
                login = fopen("login.txt", "a");
                fputs(nome_utente,login);                       //nome utente
                fflush(login);
                fputs(inet_ntoa(remote_addr.sin_addr), login);      //ip 
                fflush(login);
                fputs("\n", login);
                fflush(login);
                fputs(porta, login);                    //porta
                fflush(login);
                fputs("\n", login);
                fflush(login);
                fputs(portaTCP, login);             //porta TCP
                fflush(login);
                fputs("\n", login);
                fflush(login);
                fputs(file,login);                               //lista dei file
                fflush(login);
                fputs("\n", login);
                fflush(login);
                fclose(login);
        
            }else if(strcmp(mesg, "who") == 0 || strcmp(mesg, "who\n") == 0)    //WHO
            {  
                strcpy(ipAppoggio, inet_ntoa(remote_addr.sin_addr));
                login = fopen("login.txt", "r");
                //cerco la porta per spedire
                while(fgets(ignore, MAX_LINE, login) != NULL && fgets(ip, MAX_LINE, login) != NULL  && fgets(porta, MAX_LINE, login) != NULL && fgets(ignore, MAX_LINE, login) != NULL  && fgets(ignore, MAX_LINE, login) != NULL)
                {
                    ip[strcspn(ip, "\n")]=0; 
                    porta[strcspn(porta,"\n")]=0;
                    if(strcmp(ip, ipAppoggio) == 0){
                        porta[strcspn(porta, "\n")] = 0;            
                        portaAppoggio = atoi(porta);                   
                        break;
                    }
                }

                fclose(login);
                login = fopen("login.txt", "r");
                strcpy(mesg, "W");
                if(sockfd>0){   
                    memset(&remote_addr, 0, sizeof(remote_addr));
                    remote_addr.sin_family = AF_INET;
                    inet_pton(AF_INET, ipAppoggio, &(remote_addr.sin_addr)); 
                    remote_addr.sin_port = htons(portaAppoggio);
                    sendto(sockfd,mesg,strlen(nome),  0, (struct sockaddr *)&remote_addr, len); //invio del nome utente
                }
                while(fgets(nome, MAX_LINE, login)!= NULL && fgets(ip, MAX_LINE, login)!= NULL && fgets(ignore, MAX_LINE, login)!= NULL  && fgets(portaTCP, MAX_LINE, login)!= NULL && fgets(file, MAX_LINE, login)!= NULL)
                {
                    nome[strcspn(nome, "\n")]=0; 
                    ip[strcspn(ip, "\n")]=0;
                    portaTCP[strcspn(portaTCP, "\n")]=0;
                    file[strcspn(file, "\n")]=0;
                    printf("invio a indirizzo %s e porta %d \n", ipAppoggio, portaAppoggio);
                    
                    if(sockfd>0){   
                        memset(&remote_addr, 0, sizeof(remote_addr));
                        remote_addr.sin_family = AF_INET;
                        inet_pton(AF_INET, ipAppoggio, &(remote_addr.sin_addr)); 
                        remote_addr.sin_port = htons(portaAppoggio);

                        sendto(sockfd,nome,strlen(nome),  0, (struct sockaddr *)&remote_addr, len); //invio del nome utente
                        sendto(sockfd,ip,strlen(ip),  0, (struct sockaddr *)&remote_addr, len); //invio dell'ip
                        sendto(sockfd,portaTCP,strlen(portaTCP),  0, (struct sockaddr *)&remote_addr, len); //invio della portaTCP
                        sendto(sockfd,file,strlen(file),  0, (struct sockaddr *)&remote_addr, len); //invio della lista file
                    }
                }
                    strcpy(mesgS,"X");
                    sendto(sockfd,mesgS,strlen(file),  0, (struct sockaddr *)&remote_addr, len); //invio carattere di terminazione
                    fclose(login);
            }
        }
        return 0;
    }
    else{
        wait(NULL);
        for(;;){}
    }
    
}

         

