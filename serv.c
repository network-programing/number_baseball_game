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


void error_handle(char* msg, int exitnum){
	fprintf(stderr, "%s\n", msg);
	exit(exitnum);
}

void *handle_clnt(void* arg){
    struct client* c = (struct client*) arg;
    struct message msg;
    int str_len;
	struct room* test_room;

    sendWaitingRoomMenu(c);

    while((str_len = read(c->socket, &msg, sizeof(msg)))){
        /* client is in the gaming room */
        if(isInTheGamingRoom(c)){
            handle_clnt_msg_in_gaming(room, &room_num, c, msg);
        }
        /* client is in the wating room */
        else{
            handle_clnt_msg_in_waiting(room, &room_num, c, msg);
        }
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

	// 계속해서 접속자 받기
	while(1)
	{

		clnt_sock = accept(serv_sock, NULL, NULL);

		read(clnt_sock, &msg, sizeof(struct message));

		strcpy(name, msg.content);
		new_clnt = addClient(clnt, &clnt_num, clnt_sock, name);

		//pthread_create(&serv_id, NULL, handle_serv, (void*)new_clnt);
		pthread_create(&t_id, NULL, handle_clnt, (void*)new_clnt);

		//pthread_detach(serv_id);
		pthread_detach(t_id);

		printf("%s is connected\n", clnt->info.name);
	}
	close(serv_sock);

	return 0;
}
