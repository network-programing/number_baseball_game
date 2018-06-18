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
#include <sys/time.h>

#include "message.h"
#include "snake.h"

#define SIZE_MODE 20
#define SIZE_CONTENT 200

pthread_mutex_t msg_lock = PTHREAD_MUTEX_INITIALIZER;


int mode_num = 0;	//mode number
int pre_mode_num;
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



void handleUserInput(struct message msg, int sock, char* name){
	char buf[300];
	
	/* if send mode is chat */
	if(strcmp(msg.mode, mode[2]) == 0){
		//if(strcmp(msg.content, "\n"))

		strcpy(buf, msg.content);
		sprintf(msg.content, "user [%s] : %s\n", name, buf);
	}

	write(sock, &msg, sizeof(msg));
}


void handleServerMessage(struct message msg){
	/*
		if(strcmp(msg.content, "end") == 0 || strcmp(msg.content, "End")==0){
			close(sock);
			printf("접속을 종료합니다.\n");
			exit(0);
		}
	*/

	write(0, msg.content, strlen(msg.content));
}


void io_handle(int sock){
	fd_set reads, temps;
	int result;
	struct message msg;
	int str_len;
	struct timeval timeout;
	char buf[300], name[50];
	int score;


	while(1){
		// receive server msg;
		read(sock, &msg, sizeof(msg));
		write(0, msg.content, strlen(msg.content));

		mode_num = 0;

		// 접속할 이름 입력
		strcpy(msg.mode, mode[mode_num]);
		fgets(msg.content, SIZE_CONTENT, stdin);
		msg.content[strlen(msg.content)-1] = '\0';

		//이름 서버에게 전달
		write(sock, &msg, sizeof(struct message));
		strcpy(name, msg.content);

		// receive server's accepted msg;
		read(sock, &msg, sizeof(msg));
		if(strcmp(msg.content, "connected to server") == 0){
			break;
		}

		// not accepted
		write(0, msg.content, strlen(msg.content));
	}

	mode_num = 1;	// 처음은 SELECT 모드
	printf("\n선택모드가 되었습니다.\n선택모드, 채팅모드간 전환하려면 CTRL+C를 입력하세요\n");


	FD_ZERO(&reads);
	FD_SET(0, &reads);
	FD_SET(sock, &reads);

	while(1){
		temps = reads;

		timeout.tv_sec = 0;
		timeout.tv_usec = 300000;	/* 0.3 sec timeout */

		pre_mode_num = mode_num;

		result = select(sock + 1, &temps, 0, 0, &timeout);

		if(result == -1){
			/* if select error is not occured by SIGINT */
			if(pre_mode_num == mode_num)
				errorHandle("select() error", 1);
		}else if(result > 0){
			/* read client input */
			if(FD_ISSET(0, &temps))
			{
				memset(&msg, 0, sizeof(msg));
				read(0, msg.content, SIZE_CONTENT);

				msg.content[strlen(msg.content)-1] = '\0';		
				strcpy(msg.mode, mode[mode_num]);

				handleUserInput(msg, sock, name);
			}
			/* read server message */
			else if(FD_ISSET(sock, &temps)){
				read(sock, &msg, sizeof(msg));

				if(strcmp(msg.mode, "quit") == 0){
					break;
				}

				if(strcmp(msg.mode, "start") == 0){
					printf("Start!!!");
					score = start();
					sprintf(msg.mode, "game");
					sprintf(msg.content, "%d", score);
					write(sock, &msg, sizeof(msg));
					
					read(sock, &msg, sizeof(msg));

					printf("your score is %d\n", score);
					printf("%s", msg.content);
				}else{
					handleServerMessage(msg);
				}
			}
		}
	}
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

	io_handle(sock_fd);

	//pthread_create(&snd_thread, NULL, send_msg, (void*)&sock_fd);	
	//pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock_fd);	
	//pthread_join(snd_thread, NULL);	
	//pthread_join(rcv_thread, NULL);	
	close(sock_fd); 


	return 0;
}
