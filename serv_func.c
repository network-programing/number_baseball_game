#include "serv_func.h"

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

int make_server_socket_q(int portnum, int backlog)
{
	struct  sockaddr_in   saddr;	/* build our address here */
	struct hostent*  hp;	         	/* this is part of our           */
	char   hostname[HOSTLEN]; 	/* address                            */
	int      sock_id;		/* the socket                        */

	sock_id = socket(PF_INET, SOCK_STREAM, 0);  /* get a socket  */
	if ( sock_id == -1 ) 
		return -1;

	/** build address and bind it to socket **/
	bzero((void *)&saddr, sizeof(saddr)); 	/* clear out struct     */
	gethostname(hostname, HOSTLEN); 	/* where am I ?         */
	hp = gethostbyname(hostname);         	/* get info about host  */
	/* fill in host part    */
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(portnum);        	/* fill in socket port  */
	saddr.sin_family = AF_INET ;            	/* fill in addr family  */
	if ( bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0 )
		return -1;

	/** arrange for incoming calls **/

	if ( listen(sock_id, backlog) != 0 ) 
		return -1;
	return sock_id;
}


int make_server_socket(int portnum)
{
	return make_server_socket_q(portnum, BACKLOG);
}

