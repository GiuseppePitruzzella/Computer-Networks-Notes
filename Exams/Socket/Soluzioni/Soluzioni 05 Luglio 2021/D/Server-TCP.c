#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define DIM_BUFF 1024
#define MAX_USERS 1024
char* login(int);
void save_user(char*);
void remove_user(char*);
void send_users_online(int);
int start_server(int, struct sockaddr_in, struct sockaddr_in);
int check_command(int, char*, char*, int*);
void check_if_sent(int, int);
void append_message(char*,char*, unsigned int);

int main(int argc, char** argv){
    char usernames_online[100][100];
    int socket_online[100];
    struct sockaddr_in local_addr, remote_addr;
	socklen_t addr_len = sizeof(struct sockaddr_in);
    char buff[DIM_BUFF];

    pid_t pid = 1;
    int sock = start_server(atoi(argv[1]), local_addr, remote_addr);
	unsigned int id = 0;
    while (1){
        int new_sock = accept(sock, (struct sockaddr*) &remote_addr, &addr_len);
		id++;
        pid = fork();
		//PROCESSO FIGLIO
        if(!pid){
			close(sock);
			char* username = login(new_sock);
      		fcntl(new_sock, F_SETFL, O_NONBLOCK);
			printf("%s si è loggato con id %d\n", username, id);
			save_user(username);
			int exit = 0;
			while(!exit){
				memset(buff, 0 ,DIM_BUFF);
				int num_byte = recv(new_sock, buff, DIM_BUFF-1, 0);
				if(num_byte > 0 && !check_command(new_sock, buff, username, &exit)){
					printf("%s: %s\n", username, buff);
					append_message(buff, username, id);
				}
				check_if_sent(new_sock, id);
            }
        	close(new_sock);
			return 0;
        }
    }
    close(sock);
	return 0;
}

int start_server(int port, struct sockaddr_in local_addr, struct sockaddr_in remote_addr){
    int sock;
	socklen_t addr_len = sizeof(struct sockaddr_in);
    if( (sock = socket(AF_INET,SOCK_STREAM, 0)) < -1 ){
        perror("Errore creazione socket\n");
        exit(-1);
    }

    memset(&local_addr, 0, addr_len);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(port);
    if( bind(sock, (struct sockaddr*) &local_addr, addr_len) < -1 ){
        perror("Errore bind\n");
        exit(-1);
    }

    listen(sock,10);
	return sock;
}

char* login(int sock){
	char* buff = malloc(DIM_BUFF);
	send(sock, "Nome utente: ",13, 0);
	int num_bytes = recv(sock, buff, DIM_BUFF-1, 0);
	buff[num_bytes] = 0;
	return buff;
}

int check_command(int sock, char* command, char* username, int* exit){
	if(strcmp(command, "LOGOUT") == 0 || strcmp(command, "LOGOUT\n") == 0){
		send(sock, "LOGOUT", 6, 0);
		remove_user(username);
		(*exit) = 1;
		return 1;
	}
	if(strcmp(command, "who") == 0 || strcmp(command, "who\n") == 0){
		send_users_online(sock);
		return 1;
	}
	return 0;
}

void save_user(char* username){
	FILE* file = fopen("users.txt", "a");
	fprintf(file,"%s\n", username);
	fclose(file);
}

void remove_user(char* username){
	char usernames[100][100];
	FILE* file = fopen("users.txt", "r");
	int q = 0;
	while(!feof(file)){
		char line[100];
		fscanf(file, "%s", line);
		if(strcmp(line, username))
			memcpy(usernames[q++], line, strlen(line));
	}
	fclose(file);
	file = fopen("users.txt", "w");
	for(int i = 0; i < q; i++)
		fprintf(file, "%s\n", usernames[i]);
	fclose(file);
}

void send_users_online(int sock){
	FILE* file = fopen("users.txt", "r");
	while(!feof(file)){
		char username[100];
		fscanf(file, "%s", username);
		username[strlen(username)] = '\n';
		send(sock, username, strlen(username), 0);
		memset(username, 0, 100);
	}
	fclose(file);
}

void check_if_sent(int sock, int id){
	char buff[DIM_BUFF];
	FILE* file = fopen("msg.txt", "a+");
	fgets(buff, DIM_BUFF, file);
	if(strcmp(buff,"")){
		char* msg = strtok(buff, ",");
		char* curr_id = strtok(NULL, ",");
		while (curr_id != NULL) {
			if(atoi(curr_id) == id){
				fclose(file);
				return;
			}
			curr_id = strtok(NULL, ",");
		}
		fprintf(file, ",%d", id);
		send(sock, msg, strlen(msg), 0);
	}
		fclose(file);
}

void append_message(char* msg, char* username, unsigned int id){
	FILE* file = fopen("msg.txt", "w");
	fprintf(file, "%s: %s,%d", username, msg, id);
	fclose(file);
}

