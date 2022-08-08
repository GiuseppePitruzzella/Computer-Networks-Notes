#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 900
#define FILE_HOST "dati.txt"
#define FILE_CONDIVISI "condivisiDaiClient.txt"

using namespace std;


/*
    Descrizione del file
    
    Il server accetta le iscrizioni dei client tramite username, ottiene i dati dei client dal pacchetto ricevuto, non vengono chiesti al client
    Nel caso in cui ci siano più iscrizioni con lo stesso username, vengono cancellati tutti i duplicati dal demone, in seguito all'aggiornamento

    Il client può ottenere informazioni su:
        I file condivisi da uno specifico client
        I client effettivamente connessi
        I dati di uno specifico client

    Il receiver riceve le richieste, il demone si occupa di aggiornare i file condivisi e i client effettivamente attivi
    Viene implementata una lista solamente per gestire l'aggiornamento dei client attivi
   
    Il server si interfaccia solamente tramite UDP
*/

class Destinatario{
    char buffUsername[BUF_SIZE];
    char buffIp[BUF_SIZE];
    char buffPortUDP[BUF_SIZE];
    char buffPortTCP[BUF_SIZE];
    char buffFile[BUF_SIZE];
    
    bool attivo;

    public: 
        Destinatario(char* buffUsername, char* buffIp, char* buffPortUDP, char* buffPortTCP){
            strcpy(this->buffUsername, buffUsername);
            strcpy(this->buffIp, buffIp);
            strcpy(this->buffPortUDP, buffPortUDP);
            strcpy(this->buffPortTCP, buffPortTCP);
            
            attivo = false;
        }

        char* getUsername(){
            return buffUsername;
        }

        char* getIP(){
            return buffIp;
        }

        char* getPortUDP(){
            return buffPortUDP;
        }

        char* getPortTCP(){
            return buffPortTCP;
        }

        void setAttivo(bool newAttivo){
            attivo = newAttivo;
        }

        void setFile(char* sharedFile){
            strcpy(buffFile, sharedFile);
        }

        bool getAttivo(){
            return attivo;
        }

        char* getFile(){
            return buffFile;
        }
};

class Nodo{
    Destinatario* val;
    Nodo* succ;

    public:
        Nodo(Destinatario* val){
            this->val = val;
            succ = NULL;
        }

        Destinatario* getVal(){
            return val;
        }

        void setSucc(Nodo* newSucc){
            this->succ = newSucc;
        }

        Nodo* getSucc(){
            return succ;
        }
};

class Lista{
    Nodo* testa;

    public:
        Lista(){
            testa = NULL;
        }

        Nodo* getTesta(){
            return testa;
        }

        void inserimentoTesta(Destinatario* newData){
			Nodo* newNodo = new Nodo(newData);
		
			if(testa == NULL)
				testa = newNodo;
			else{
				newNodo->setSucc(testa);
				testa = newNodo;
			}
		}

        int getLength(){
            Nodo* app = testa;
            int i;

            for(i = 0; app != NULL; i++){
                app = app->getSucc();
            }

            return i;
        }

        void setTrue(char* username){
            int listLength = getLength();
            Nodo* app = testa;

            while(app != NULL){
                if(strcmp(app->getVal()->getUsername(), username) == 0){
                    app->getVal()->setAttivo(true);
                    return;
                }

                app = app->getSucc();
            }
        }

        void setFile(char* username, char* sharedFile){
            int listLength = getLength();
            Nodo* app = testa;

            while(app != NULL){
                if(strcmp(app->getVal()->getUsername(), username) == 0){
                    app->getVal()->setFile(sharedFile);
                    return;
                }

                app = app->getSucc();
            }
        }

        bool getAttivo(int index){
            Nodo* app = testa;

            for(int i = 0; i < index; i++){
                app = app->getSucc();
            }

            return app->getVal()->getAttivo();
        }

        Nodo* getNodoByIndex(int index){
            Nodo* app = testa;
            for(int i = 0; i < index && app != NULL; i++)
                app = app->getSucc();
            
            return app;
        }
};

void sendToFromFile(char* username, char* messaggio, int idSock){
    FILE* fileHosts = fopen(FILE_HOST, "r");
    struct sockaddr_in dest;
    
    char buffUsername[BUF_SIZE];
    char buffIp[BUF_SIZE];
    char buffPortUDP[BUF_SIZE];
    char buffPortTCP[BUF_SIZE];
    
    while(!feof(fileHosts)){
        fscanf(fileHosts, "%s", buffUsername);
        fscanf(fileHosts, "%s", buffIp);
        fscanf(fileHosts, "%s", buffPortUDP);
        fscanf(fileHosts, "%s", buffPortTCP);

        if(strcmp(username, buffUsername) == 0){
            memset(&dest, 0, sizeof(struct sockaddr_in));
            dest.sin_family = AF_INET;
            dest.sin_port = htons(atoi(buffPortUDP));
            inet_pton(AF_INET, buffIp, &(dest.sin_addr));

            sendto(idSock, messaggio, strlen(messaggio), 0, (struct sockaddr *) &dest, sizeof(dest));
            fclose(fileHosts);
            break; 
        }
    }
}

void gestisciWho(FILE* fileHosts, int idSock, char* username){
    char output[BUF_SIZE * 4 + 2];
    
    strcpy(output, "Utenti Connessi");
    sendToFromFile(username, output, idSock);
    memset(output, 0, BUF_SIZE);

    char buffUsername[BUF_SIZE];
    char buffIp[BUF_SIZE];
    char buffPortUDP[BUF_SIZE];
    char buffPortTCP[BUF_SIZE];

    while(!feof(fileHosts)){
        fscanf(fileHosts, "%s", buffUsername);
        fscanf(fileHosts, "%s", buffIp);
        fscanf(fileHosts, "%s", buffPortUDP);
        fscanf(fileHosts, "%s", buffPortTCP);

        sprintf(output, "%s,%s,%s,%s", buffUsername, buffIp, buffPortUDP, buffPortTCP);
        sendToFromFile(username, output, idSock);
    }
}

void gestisciInvioShared(FILE* fileShared, int idSock, char* usernameRichiedente, char* usernameCondivisore){
    char buffRicezione[BUF_SIZE];
    char buffOutput[BUF_SIZE];
    char* buffNomeFile;
    char* buffNomeCondivisore;

    while(!feof(fileShared)){
        fgets(buffRicezione, BUF_SIZE, fileShared);
        buffNomeCondivisore = strtok(buffRicezione, ",");
        buffNomeFile = strtok(NULL, "\n");

        if(strcmp(buffNomeCondivisore, usernameCondivisore) == 0){
            sprintf(buffOutput, "%s %s:%s", "File condivisi da", buffNomeCondivisore, buffNomeFile);
            sendToFromFile(usernameRichiedente, buffOutput, idSock);
            break;
        }
    }
}

void gestisciInvioDestinatario(FILE* fileHosts, int idSock, char* usernameRichiedente, char* usernameRichiesto){
    char output[BUF_SIZE * 4 + 2];
    memset(output, 0, BUF_SIZE);

    char buffUsername[BUF_SIZE];
    char buffIp[BUF_SIZE];
    char buffPortUDP[BUF_SIZE];
    char buffPortTCP[BUF_SIZE];

    while(!feof(fileHosts)){
        fscanf(fileHosts, "%s", buffUsername);
        fscanf(fileHosts, "%s", buffIp);
        fscanf(fileHosts, "%s", buffPortUDP);
        fscanf(fileHosts, "%s", buffPortTCP);

        if(strcmp(usernameRichiesto, buffUsername) == 0){
            sprintf(output, "%s,%s,%s,%s", buffUsername, buffIp, buffPortUDP, buffPortTCP);
            sendToFromFile(usernameRichiedente, output, idSock);
            break;
        }
    }
}

int main(){    
    struct sockaddr_in datiHost;
    int portaAscolto;

    FILE* fileCondivisi = fopen(FILE_CONDIVISI, "w+");
    FILE* fileHosts = fopen(FILE_HOST, "w+");

    fclose(fileCondivisi);
    fclose(fileHosts);

    int idSock;

    char buffInvio[BUF_SIZE];
    char buffRicezione[BUF_SIZE];
    char* buffMessaggio2;
    char* buffMessaggio;
    char* buffSegnale;

    socklen_t len = sizeof(sockaddr_in);

    cout << "Scegli una porta di ascolto" << endl;
    cin >> portaAscolto;
    
    memset(&datiHost, 0, sizeof(struct sockaddr_in));
    datiHost.sin_port = htons(portaAscolto);
    datiHost.sin_family = AF_INET;

    //Demone che si occupa di aggiornare i client connessi
    if(!fork()){
        Lista* lista;

        char buffUsername[BUF_SIZE];
        char buffIp[BUF_SIZE];
        char buffPortUDP[BUF_SIZE];
        char buffPortTCP[BUF_SIZE];
        char buffOutput[BUF_SIZE];

        char* usernameInput;
        char* fileListInput;

        int n;

        if((idSock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
            cout << "Errore nella creazione della socket sender" << endl;
            return -1;
        }
        
        while(1){
            sleep(20);

            lista = new Lista();
            cout << "Aggiorno i client connessi..." << endl;
            
            memset(buffRicezione, 0, BUF_SIZE);

            fileHosts = fopen(FILE_HOST, "r");
            Nodo* app; 
            
            while(!feof(fileHosts)){
                fscanf(fileHosts, "%s", buffUsername);
                fscanf(fileHosts, "%s", buffIp);
                fscanf(fileHosts, "%s", buffPortUDP);
                fscanf(fileHosts, "%s", buffPortTCP);

                lista->inserimentoTesta(new Destinatario(buffUsername, buffIp, buffPortUDP, buffPortTCP));
                strcpy(buffInvio, "<checkAttivo>");
                sendToFromFile(buffUsername, buffInvio, idSock);
            }

            sleep(5);

            while((n = recvfrom(idSock, buffRicezione, BUF_SIZE, MSG_DONTWAIT, (struct sockaddr*) &datiHost, &len)) != -1){
                usernameInput = strtok(buffRicezione, ",");
                fileListInput = strtok(NULL, "\n");
                sprintf(buffOutput, "%s,%s\n", usernameInput, fileListInput);

                lista->setFile(usernameInput, buffOutput);
                lista->setTrue(usernameInput);
            }

            fclose(fileHosts);
            while((fileHosts = fopen(FILE_HOST, "w")) < 0);
            while((fileCondivisi = fopen(FILE_CONDIVISI, "w")) < 0);

            for(int i = 0; i < lista->getLength(); i++){
                app = lista->getNodoByIndex(i);

                if(app != NULL && app->getVal()->getAttivo()){
                    fprintf(fileHosts, "%s\n%s\n%s\n%s\n", app->getVal()->getUsername(), app->getVal()->getIP(), app->getVal()->getPortUDP(), app->getVal()->getPortTCP());
                    fprintf(fileCondivisi, "%s", app->getVal()->getFile());
                    fflush(fileHosts);
                    fflush(fileCondivisi);
                }
            }

            fclose(fileHosts);
            fclose(fileCondivisi);
        }
    }
    //Receiver, si interfaccia con i client
    else{
        if((idSock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
            cout << "Errore nella creazione della socket sender" << endl;
            return -1;
        }

        if(bind(idSock, (struct sockaddr*) &datiHost, len) < 0){
            cout << "Errore nella bind" << endl;
            return 0;
        }

        while(1){
            recvfrom(idSock, buffRicezione, BUF_SIZE, 0, (struct sockaddr*) &datiHost, &len);
            cout << "Messaggio da " << inet_ntoa(datiHost.sin_addr) << " port " << ntohs(datiHost.sin_port) << endl;
            
            buffSegnale = strtok(buffRicezione, ",");

            /*
                File con struttura:
                username
                indirizzoIP
                portaUDP
                portaTCP
            */
            if(strcmp(buffSegnale, "<subscribing>") == 0){
                fileHosts = fopen(FILE_HOST, "a");
                buffMessaggio = strtok(NULL, ",");
                buffMessaggio2 = strtok(NULL, "\n");

                fprintf(fileHosts, "%s\n%s\n%d\n%s\n", buffMessaggio, inet_ntoa(datiHost.sin_addr), ntohs(datiHost.sin_port), buffMessaggio2);
                fflush(fileHosts);
                sendto(idSock, "Iscritto!", BUF_SIZE, 0, (struct sockaddr*) &datiHost, sizeof(datiHost));
                fclose(fileHosts);
            }
            else if(strcmp(buffSegnale, "<who>") == 0){
                fileHosts = fopen(FILE_HOST, "r");
                buffMessaggio = strtok(NULL, "\n");
                gestisciWho(fileHosts, idSock, buffMessaggio);
                fclose(fileHosts);
            }
            else if(strcmp(buffSegnale, "<getData>") == 0){
                fileHosts = fopen(FILE_HOST, "r");

                buffMessaggio = strtok(NULL, ",");
                buffMessaggio2 = strtok(NULL, "\n");
                gestisciInvioDestinatario(fileHosts, idSock, buffMessaggio, buffMessaggio2);

                fclose(fileHosts);
            }
            else if(strcmp(buffSegnale, "<getSharedFiles>") == 0){
                fileCondivisi = fopen(FILE_CONDIVISI, "r");
                
                buffMessaggio = strtok(NULL, ",");
                buffMessaggio2 = strtok(NULL, "\n");

                gestisciInvioShared(fileCondivisi, idSock, buffMessaggio, buffMessaggio2);

                fclose(fileCondivisi);
            }
        }
    }
}