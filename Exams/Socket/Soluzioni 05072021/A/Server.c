//Francesco Cerruto 1000005927

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DIM_LINE 1000

int main(int argc, char* argv[]) {
    int sockfd, newsockfd, sendsockfd, n;
    struct sockaddr_in local_addr, remote_addr;
    socklen_t len;
    char msg[DIM_LINE];
    char sendline[DIM_LINE];
    char mittente[DIM_LINE];
    char ip_mittente[DIM_LINE];
    char porta_mittente[DIM_LINE];
    char username[DIM_LINE];
    char porta[DIM_LINE];
    char ip[DIM_LINE];

    FILE* login;
    login = fopen("login.txt", "w");
    fclose(login);

    if (argc<2) {
        fprintf(stderr, "Parametri non validi \n");
        exit(1);
    }

    if ((sockfd=socket(PF_INET, SOCK_STREAM, 0))<0) {
        fprintf(stderr, "Errore socket\n");
        exit(1);
    }

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (struct sock_addr*) &local_addr, sizeof(local_addr))<0) {
        fprintf(stderr, "Errore bind\n");
        exit(1);
    }

    //connessione tcp
    listen(sockfd, 5);
    for (;;) {
        len = sizeof(remote_addr);

        newsockfd = accept(sockfd, (struct sock_addr*) &remote_addr, &len);

        //connessione stabilita

        if (fork()==0) {
            close(sockfd);
            login = fopen("login.txt", "a");

            n = recv(newsockfd, msg, DIM_LINE-1, 0);
            msg[n]='\0';

            fputs(msg,login);   //nome utente scritto
            fflush(login);
            n = recv(newsockfd, msg, DIM_LINE-1, 0);
            msg[n]='\0';
            fputs(msg,login);   //porta scritta
            fflush(login);
            fputs("\n", login);
            fputs(inet_ntoa(remote_addr.sin_addr),login);
            fputs("\n", login); //ip scritto
            fflush(login);

            fclose(login);

            for (;;) {
                
                n = recv(newsockfd, msg, DIM_LINE-1, 0);
                msg[n]='\0';
                
                if (strstr(msg, "who")!=NULL) {

                    login = fopen("login.txt", "r");
                    while (fgets(sendline, DIM_LINE, login)!=NULL) {
                        send(newsockfd, sendline, strlen(sendline), 0);

                        fgets(sendline, DIM_LINE, login);
                        fgets(sendline, DIM_LINE, login);
                    }

                    strncpy(sendline, "FINE DEGLI UTENTI CONNESSI\n", DIM_LINE);
                    send(newsockfd, sendline, strlen(sendline), 0);

                    fclose(login);
                }
                else {
                    if (strstr(msg, "logout")!=NULL) {
                    
                        FILE* logout;
                        logout = fopen("logout.txt", "w");
                        login = fopen("login.txt", "r");
                        while (fgets(username, DIM_LINE, login)!=NULL && fgets(porta, DIM_LINE, login)!=NULL && fgets(ip, DIM_LINE, login)!=NULL) {
                            ip[strlen(ip)-1]='\0';
                            if (strcmp(inet_ntoa(remote_addr.sin_addr), ip)!=0) {
                                fputs(username, logout);
                                fflush(logout);
                                fputs(porta, logout);
                                fflush(logout);
                                fputs(ip, logout);
                                fflush(logout);
                                fputs("\n", logout);
                                fflush(logout);
                            }
                        }
                        fclose(logout);
                        fclose(login);
                        logout = fopen("logout.txt", "r");
                        login = fopen("login.txt", "w");
                        while (fgets(username, DIM_LINE, logout)!=NULL && fgets(porta, DIM_LINE, logout)!=NULL && fgets(ip, DIM_LINE, logout)!=NULL) {
                            fputs(username, login);
                            fflush(login);
                            fputs(porta, login);
                            fflush(login);
                            fputs(ip, login);
                            fflush(login);
                        }
                        fclose(logout);
                        fclose(login);
                        exit(0);
                    }
                    else {
                    
                        login = fopen("login.txt", "r");

                        //cerco mittente
                        while (fgets(username, DIM_LINE, login)!=NULL && fgets(porta, DIM_LINE, login)!=NULL && fgets(ip, DIM_LINE, login)!=NULL) {
                            ip[strcspn(ip, "\n")]=0;
                            if (strcmp(inet_ntoa(remote_addr.sin_addr), ip)==0) {
                                strncpy(mittente, username, DIM_LINE);
                                strncpy(ip_mittente, ip, DIM_LINE);
                                mittente[strlen(mittente)-1]='\0';
                                break;
                            }
                        }
                        
                        fclose(login);
                        login = fopen ("login.txt", "r");
                        
                        //cerco altri client
                        while (fgets(username, DIM_LINE, login)!=NULL && fgets(porta, DIM_LINE, login)!=NULL && fgets(ip, DIM_LINE, login)!=NULL) {
                            ip[strcspn(ip, "\n")]=0;
                            porta[strcspn(porta, "\n")]=0;
                            
                            if (strcmp(ip_mittente, ip)!=0) {
                                strncpy(sendline, mittente, DIM_LINE);
                                strncpy(sendline+strlen(sendline), ": ", DIM_LINE-strlen(sendline));
                                strncpy(sendline+strlen(sendline), msg, DIM_LINE-strlen(sendline));

                                sendsockfd=socket(AF_INET, SOCK_STREAM, 0);
                                if (sendsockfd>0) {

                                    memset(&remote_addr, 0, sizeof(remote_addr));
                                    remote_addr.sin_family = AF_INET;
                                    remote_addr.sin_addr.s_addr = inet_addr(ip);
                                    remote_addr.sin_port = htons(atoi(porta));

                                    connect(sendsockfd, (struct sock_adrr*) &remote_addr, sizeof(remote_addr));

                                    send(sendsockfd, sendline, strlen(sendline), 0);
                                    close(sendsockfd);
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            close(newsockfd);
        }
    }
}