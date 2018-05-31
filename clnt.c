#include <stdio.h>
#include <unistd.h>
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

#include "message.h"

#define SIZE_MODE 20
#define SIZE_CONTENT 200

pthread_mutex_t msg_lock = PTHREAD_MUTEX_INITIALIZER;


int mode_num = 0;	//mode number
char mode[5][SIZE_MODE] = {"new", "select", "chat"};	//서버에게 보낼 클라이언트의 메뉴
int money;



void errorHandle(char* msg, int exitnum){
	fprintf(stderr, "%s\n", msg);
	exit(exitnum);
}

int connect_to_server(char *host, int portnum)
{
	int                               sock;
	struct sockaddr_in  servadd;	/* the number to call */
	struct hostent           *hp;            	/* used to get number */
	struct message msg_send;

	/** Step 1: Get a socket **/
	sock = socket( AF_INET, SOCK_STREAM, 0 );    /* get a line   */
	if ( sock == -1 ) 
		return -1;

	/** Step 2: connect to server **/
	memset(&servadd, 0, sizeof(servadd));
	servadd.sin_addr.s_addr = inet_addr(host);
	servadd.sin_port = htons(portnum);	/* fill in port number  */
	servadd.sin_family = AF_INET ;          	/* fill in socket type  */

	if ( connect(sock,(struct sockaddr *)&servadd, sizeof(servadd)) !=0)
		return -1;


	return sock;
}


/*	서버에게 메시지를 보내는 함수	*/
void* send_msg(void* arg)
{
	int sock = *((int*)arg);
	struct message msg;
	int m;
	char option[20], buf[300], name[100];

	// 접속할 이름 입력
	printf("이름을 입력하세요.\n");
	mode_num = 0;
	strcpy(msg.mode, mode[mode_num]);
	fgets(msg.content, SIZE_CONTENT, stdin);
	msg.content[strlen(msg.content)-1] = '\0';

	//이름 서버에게 전달
	write(sock, &msg, sizeof(struct message));
	strcpy(name, msg.content);
	usleep(500000);

	mode_num = 1;	// 처음은 SELECT 모드
	printf("\n선택모드가 되었습니다.\n선택모드, 채팅모드간 전환하려면 CTRL+C를 입력하세요\n");

	while(1)
	{
		usleep(1000000);
		
		while(fgets(msg.content, SIZE_CONTENT, stdin) == NULL);

		msg.content[strlen(msg.content)-1] = '\0';		
		strcpy(msg.mode, mode[mode_num]);

		/* if send mode is chat */
		if(strcmp(msg.mode, mode[2]) == 0){
			//if(strcmp(msg.content, "\n"))

			strcpy(buf, msg.content);
			sprintf(msg.content, "user [%s] : %s", name, buf);
		}

		write(sock, &msg, sizeof(msg));
	}

	return NULL;
}

/*	서버에게서 메시지를 전달 받는 함수	*/
void* recv_msg(void* arg){
	int sock = *((int*)arg);
	struct message msg;
	int m;
	char buf[100];

	while(1){
		usleep(1000000);
		while(read(sock, &msg, sizeof(msg)) <= 0);
		/*
		if(strcmp(msg.content, "end") == 0 || strcmp(msg.content, "End")==0){
			close(sock);
			printf("접속을 종료합니다.\n");
			exit(0);
		}
		*/

		fputs(msg.content, stdout);
	}

	return NULL;
}

void keycontrol(int sig)
/*
	클라이언트가 SIGINT를 누르면 select와 chat이 번갈아 가면서 바뀜
*/
{
	if(sig == SIGINT){
		if(mode_num == 1){
			mode_num = 2;
			printf("\n채팅모드로 변경하였습니다.\n");
		}else if(mode_num == 2){
			mode_num = 1;
			printf("\n선택모드로 변경하였습니다.\n옵션을 보려면 -help를 입력하세요\n");
		}
	} 
}

int main(int argc, char* argv[]){
	pthread_t snd_thread, rcv_thread;
	int sock_fd;	//서버와 통신할 수 있는 소켓 fd

	if(argc != 3)
		errorHandle("Usage : 실행파일 <IP> <PORT>\n", 1);

	sock_fd = connect_to_server(argv[1], atoi(argv[2]));
	if(sock_fd == -1){
		printf("cannot connect to server\n");
		exit(1);
	}

	signal(SIGINT, keycontrol);

	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock_fd);	
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock_fd);	
	pthread_join(snd_thread, NULL);	
	pthread_join(rcv_thread, NULL);	
	close(sock_fd); 


	return 0;
}
