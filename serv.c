#include "clnt_info.h"
#include "common.h"
#include "message.h"
#include "room.h"
#include "serv_func.h"
#include "struct.h"
#include "handle_clnt_msg.h"
#include "struct.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>


#define ROOM_MAX 100
#define CLIENT_MAX 300


struct room room[ROOM_MAX];
int room_num = 0;

struct client clnt[CLIENT_MAX];
int clnt_num = 0;

struct info user_info[1000];
int info_num;


void error_handle(char* msg, int exitnum){
	fprintf(stderr, "%s\n", msg);
	exit(exitnum);
}

void *handle_clnt(void* arg){
    struct client* c = (struct client*) arg;
    struct message msg;
    int str_len, clnt_socket;
	struct room* test_room;

	clnt_socket = c->socket;
    sendWaitingRoomMenu(c);

    while((str_len = read(c->socket, &msg, sizeof(msg)))){
        /* client is in the gaming room */
        if(isInTheGamingRoom(c)){
            handle_clnt_msg_in_gaming(user_info, &info_num, room, &room_num, clnt, &clnt_num,  &c, msg, &clnt_socket);
        }
        /* client is in the wating room */
        else{
            handle_clnt_msg_in_waiting(user_info, &info_num, room, &room_num, clnt, &clnt_num, &c, msg, &clnt_socket);
        }
    }
}


void serverEndControl(int sig){
	int i;
	struct message serv_msg;
	struct client* tm_clnt;

	if(sig == SIGINT){
		printf("[server] terminate\n");

		for(i=0; i<clnt_num; i++){
			tm_clnt = &(clnt[i]);

			/* send quit message to user */
        	strcpy(serv_msg.mode, "quit");
        	strcpy(serv_msg.content, "quit");

			sendMessageUser(serv_msg, tm_clnt);

        	printf("send quit message [%s]\n", tm_clnt->info.name);
        
        	/* save client info */
        	updateClientInfo(user_info, info_num, tm_clnt, tm_clnt->info);
		}	

		exit(0);
	}
}


int main(int argc, char* argv[]){
    int serv_sock, clnt_sock;
	char name[200];	
	struct client* new_clnt;
	struct message msg;

	pthread_t t_id;			
	pthread_t serv_id;

	if(argc != 2)
		error_handle("Usage : 실행파일 <PORT>\n", 1);

	//signal(SIGPIPE, sig_handler);

	// 서버 소켓 만들기
	serv_sock = make_server_socket(atoi(argv[1]));
	if(serv_sock == -1)
		error_handle("server sock failed", 1);

	srand((long)time(NULL));

	/* read user info */
	readUserInfo(user_info, &info_num);
	printf("[user info] num is %d\n", info_num);

	signal(SIGINT, serverEndControl);

	// 계속해서 접속자 받기
	while(1)
	{

		clnt_sock = accept(serv_sock, NULL, NULL);


		do{
			memset(&msg, 0, sizeof(msg));
			sprintf(msg.content, "please input your name: ");
			write(clnt_sock, &msg, sizeof(msg));

			read(clnt_sock, &msg, sizeof(struct message));
			strcpy(name, msg.content);

			if(findClientWithName(clnt, clnt_num, name) == -1){
				break;
			}

			memset(&msg, 0, sizeof(msg));
			sprintf(msg.content, "%s is already connected\n", name);
			write(clnt_sock, &msg, sizeof(msg));
		}while(1);

		memset(&msg, 0, sizeof(msg));
		sprintf(msg.content, "connected to server");
		write(clnt_sock, &msg, sizeof(msg));

		new_clnt = addClient(user_info, &info_num, clnt, &clnt_num, clnt_sock, name);

		//pthread_create(&serv_id, NULL, handle_serv, (void*)new_clnt);
		pthread_create(&t_id, NULL, handle_clnt, (void*)new_clnt);

		//pthread_detach(serv_id);
		pthread_detach(t_id);

		printf("[%s] is connected\n", new_clnt->info.name);
	}
	close(serv_sock);

	return 0;
}
