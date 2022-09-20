#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 900
#define FILE_CONDIVISI "fileCondivisi.txt"

using namespace std;

/*
    Descrizione del file

    Il sender manda solo le richieste, le risposte vengono ricevute dal receiver
    Il receiver, oltre a ricevere le risposte dal server, si interfaccia col demone per l'aggiornamento dei client attivi
    
    I file che vogliono essere condivisi vengono mantenuti in un file "fileCondivisi"
    Il file "fileCondivisi" può essere modificato con un'apposita sezione nel menù

    Le connessioni TCP vengono aperte su richiesta e vengono chiuse immediatamente una volta inviato il file richiesto
    Quando si vuole ricevere un nuovo file viene fatta una richiesta al server per ottenere i dati (Indirizzo IP, PORT) del client cercato
    
    Un client non condivide mai file che non sono contenuti in "fileCondivisi", questa proprietà viene mantenuta tramite un'apposita ricerca nel file "fileCondivisi"
*/

bool cercaTraCondivisi(char* nomeFile){
    FILE* fileCondivisi = fopen(FILE_CONDIVISI, "r");
    char input[BUF_SIZE];

    while(!feof(fileCondivisi)){
        fscanf(fileCondivisi, "%s", input);
        if(strcmp(nomeFile, input) == 0)
            return true;
    }

    return false;
}

int main(int argc, char* argv[]){
    struct sockaddr_in datiHost;
    struct sockaddr_in datiHostTCP;
    struct sockaddr_in datiRemoteTCP;

    int scelta;
    int idSock;
    int idSockTCP;

    char buffInvio[BUF_SIZE + 2];
    char buffInvio2[BUF_SIZE * 3];
    char buffRicezione[BUF_SIZE];

    char input[BUF_SIZE * 2];

    FILE* condivisi = fopen(FILE_CONDIVISI, "w");
    FILE* fileRicevuto;
    fclose(condivisi);

    socklen_t len = sizeof(struct sockaddr_in);
    
    if(argc < 6){
        cout << "Use: Client.cpp serverIP serverPORT listenerUdpPort listenerTcpPort username" << endl;
        exit(-1);
    }

    memset(&datiHost, 0, sizeof(struct sockaddr_in));
    datiHost.sin_port = htons(atoi(argv[3]));
    datiHost.sin_family = AF_INET;

    if((idSock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        cout << "Errore nella creazione della socket sender" << endl;
        return -1;
    }

    if(bind(idSock, (struct sockaddr*) &datiHost, len) < 0){
        cout << "Errore nella bind" << endl;
        return 0;
    }

    if(!fork()){
        int newIdSock;
        FILE* fileDaInviare;

        memset(&datiHostTCP, 0, sizeof(struct sockaddr_in));
        datiHostTCP.sin_family = AF_INET;
        datiHostTCP.sin_addr.s_addr = htonl(INADDR_ANY);
        datiHostTCP.sin_port = htons(atoi(argv[4]));

        if((idSock=socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
            cout << "Errore nell'apertura del socket" << endl;
            return -1;
        }

        if(bind(idSock, (struct sockaddr *) &datiHostTCP, sizeof(datiHostTCP)) < 0){ 
            cout << "Errore nel binding TCP" << endl;
            return -1;
        }

        while(1){
            listen(idSock, 5);
            len = sizeof(datiHostTCP);
            newIdSock = accept(idSock,(struct sockaddr*) &datiHostTCP, &len);

            if(fork() == 0){
                close(idSock);
                recv(newIdSock, buffRicezione, BUF_SIZE, 0);

                //Trova il file
                if(cercaTraCondivisi(buffRicezione)){
                    fileDaInviare = fopen(buffRicezione, "r");

                    while(!feof(fileDaInviare)){
                        fgets(buffRicezione, BUF_SIZE, fileDaInviare);
                        send(newIdSock, buffRicezione, BUF_SIZE, 0);
                    }

                    cout << "Finito di inviare" << endl;
                    send(newIdSock, "<fineFile>", BUF_SIZE, 0);
                }
                else{
                    send(newIdSock, "<File non trovato>", BUF_SIZE, 0);
                }

                return 0;
            }
            else
                close(newIdSock);
        }
    }

    //Sender che si interfaccia con il server, invia le richieste che verranno poi ricevute dal receiver
    if(!fork()){
        memset(&datiHost, 0, sizeof(struct sockaddr_in));
        datiHost.sin_family = AF_INET;
        datiHost.sin_port = htons(atoi(argv[2]));
        inet_pton(AF_INET, argv[1], &(datiHost.sin_addr)); 

        while(1){
            cout << "Scegli cosa fare" << endl;
            cout << "1. Iscriviti al server" << endl;
            cout << "2. Ottieni la lista dei client collegati al server" << endl;
            cout << "3. Scarica un file da un client" << endl;
            cout << "4. Ottieni la lista dei file condivisi dagli altri client" << endl;
            cout << "5. Modifica la lista dei file condivisi" << endl;
            cin >> scelta;

            memset(&buffInvio, 0, sizeof(buffInvio));
            memset(&buffInvio2, 0, sizeof(buffInvio2));
            memset(&buffRicezione, 0, sizeof(buffRicezione));

            if(scelta == 1){
                sprintf(buffInvio, "%s,%s,%s", "<subscribing>", argv[5], argv[4]);
                sendto(idSock, buffInvio, sizeof(buffInvio), 0, (struct sockaddr*) &datiHost, sizeof(datiHost));
            }
            else if(scelta == 2){
                sprintf(buffInvio, "%s,%s", "<who>", argv[5]);
                sendto(idSock, buffInvio, sizeof(buffInvio), 0, (struct sockaddr*) &datiHost, sizeof(datiHost));
            }
            else if(scelta == 3){
                cout << "Inserisci l'username dell'utente di cui vuoi ricevere i dati" << endl;
                cin >> input;

                sprintf(buffInvio2, "%s,%s,%s", "<getData>", argv[5], input);
                sendto(idSock, buffInvio2, sizeof(buffInvio), 0, (struct sockaddr*) &datiHost, sizeof(datiHost));

                memset(&datiRemoteTCP, 0, sizeof(datiRemoteTCP));
                datiRemoteTCP.sin_family = AF_INET;

                cout << "Inserisci l'indirizzo IP dell'utente" << endl;
                cin >> input;
                datiRemoteTCP.sin_addr.s_addr = inet_addr(input);
                
                cout << "Inserisci la porta di ascolto dell'utente" << endl;
                cin >> input;
                datiRemoteTCP.sin_port = htons(atoi(input));

                cout << "Inserisci il nome del file che vuoi ricevere" << endl;
                cin >> input;
                
                idSockTCP = socket(AF_INET, SOCK_STREAM, 0);
                connect(idSockTCP, (struct sockaddr *) &datiRemoteTCP, sizeof(datiRemoteTCP));

                send(idSockTCP, input, strlen(input), 0);
                recv(idSockTCP, buffRicezione, BUF_SIZE, 0);

                if(strcmp(buffRicezione, "<File non trovato>") != 0){
                    fileRicevuto = fopen(input, "w");
                    while(1){
                        recv(idSockTCP, buffRicezione, BUF_SIZE, 0);
                        if(strcmp(buffRicezione, "<fineFile>") == 0)
                            break;
                        fprintf(fileRicevuto, "%s\n", buffRicezione);
                    }

                    fclose(fileRicevuto);
                }

                cout << "Fine ricezione" << endl;
                close(idSockTCP);
            }
            else if(scelta == 4){
                cout << "Inserisci l'username dell'utente di cui vuoi ricevere i file" << endl;
                cin >> buffInvio;

                sprintf(input, "%s,%s,%s", "<getSharedFiles>", argv[5], buffInvio);
                sendto(idSock, input, sizeof(input), 0, (struct sockaddr*) &datiHost, sizeof(datiHost));
            }
            else if(scelta == 5){
                cout << "Aggiornamento della lista dei file condivisi, inserisci -1 per terminare" << endl;
                
                while(1){
                    cin >> input;
                    
                    if(strcmp(input, "-1") == 0)
                        break;
                
                    strcat(buffInvio, input);
                    strcat(buffInvio, " ");
                }

                condivisi = fopen(FILE_CONDIVISI, "w");
                fprintf(condivisi, "%s\n", buffInvio);
                
                fflush(condivisi);
                fclose(condivisi);
            }
        }
    }
    //Receiver sempre in ascolto, si interfaccia con il demone per l'aggiornamento dei client
    else{
        while(1){
            memset(buffRicezione, 0, BUF_SIZE);
            recvfrom(idSock, buffRicezione, BUF_SIZE, 0, (struct sockaddr*) &datiHost, &len);

            if(strcmp(buffRicezione, "<checkAttivo>") == 0){
                condivisi = fopen(FILE_CONDIVISI, "r");

                memset(buffInvio, 0, BUF_SIZE);
                memset(input, 0, BUF_SIZE);

                if(fgets(buffInvio, BUF_SIZE, condivisi) != NULL)
                    sprintf(input, "%s,%s", argv[5], buffInvio);
                else
                    sprintf(input, "%s,%s", argv[5], " \n");
                
                sendto(idSock, input, BUF_SIZE, 0, (struct sockaddr*) &datiHost, sizeof(datiHost));
                fclose(condivisi);
            }
            else
                cout << "Messaggio: " << buffRicezione << endl;
        }
    }
}