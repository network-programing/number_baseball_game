#include "clnt.h"
#include "common.h"
#include "message.h"
#include "room.h"
#include "serv_func.h"


void error_handle(char* msg, int exitnum);






void *handle_clnt(void* arg){
    struct client* clnt = (struct client*) arg;
    struct message msg;
    struct buf1[100], buf2[100];
    int str_len;

    sendWaitingRoomMenu(clnt);

    while((str_len = read(clnt->socket, &msg, sizeof(msg)))){
        /* client is in the gaming room */
        if(isInTheGamingRoom(clnt)){
            handle_clnt_msg_in_gaming(clnt, msg);
        }
        /* client is in the wating room */
        else{
            handle_clnt_msg_in_waiting(clnt, msg);
        }
    }
}


int main(int argc, char* argv[]){
    int serv_sock, clnt_sock;
	char name[200];	
	struct client* client;
	struct message msg;

	pthread_t t_id;			
	pthread_t serv_id;

	if(argc != 2)
		error_handle("Usage : 실행파일 <PORT>\n", 1);

	signal(SIGPIPE, sig_handler);

	pthread_mutex_init(&lock, NULL);	

	// 서버 소켓 만들기
	serv_sock = make_server_socket(atoi(argv[1]));
	if(serv_sock == -1)
		error_handle("server sock failed\n", 1);

	srand((long)time(NULL));

	// 계속해서 접속자 받기
	while(1)
	{

		clnt_sock = accept(serv_sock, NULL, NULL);

		read(clnt_sock, &msg, sizeof(struct message));

		strcpy(name, msg.content);
		client = addClient(clnt_sock, name);
		client->money = 1000;

		//pthread_create(&serv_id, NULL, handle_serv, (void*)client);
		pthread_create(&t_id, NULL, handle_clnt, (void*)client);

		//pthread_detach(serv_id);
		pthread_detach(t_id);

		printf("%s is connected\n", client->name);
	}
	close(serv_sock);

	return 0;
}

void error_handle(char* msg, int exitnum){
	fprintf(stderr, "msg");
	exit(exitnum);
}