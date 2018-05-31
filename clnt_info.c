#include "clnt_info.h"
#include "struct.h"
#include "message.h"
#include "room.h"

#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>


pthread_mutex_t clnt_lock = PTHREAD_MUTEX_INITIALIZER;


/*

    client functions

*/



void clnt_to_str(struct client *clnt, char str[]){
    int str_len;
    char buf[50];

    sprintf(str, "--------[%s] client's info-------\n", clnt->info.name);
    
    sprintf(buf, "[%3d] wins / [%3d] loses\n", clnt->info.win, clnt->info.lose);
    strcat(str, buf);
}


int set_nonblock_socket(int fd){
    int flags;
    if((flags = fcntl(fd, F_GETFL, 0)) == -1){
        return -1;
    }

    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return 0;
}


int saveClientInfo(struct client* clnt){
    FILE* fp;
    char fileName[100];
    int fd;
    
    /* file open */
    sprintf(fileName, "%s", clnt->info.name);
    strcat(fileName, ".bin");

    if((fp = fopen(fileName, "wb")) == NULL){
        return -1;
    }

    fd = fileno(fp);
    
    /*
    if(set_nonblock_socket(fd) == -1)
        return -1;
    */

    write(fd, &(clnt->info), sizeof(struct info));

    close(fd);

    return 0;
}


struct info getClientInfo(char* name){
    struct info info;
    FILE* fp;
    char fileName[100];

    /* read user info file */
    sprintf(fileName, "%s", name);
    strcat(fileName, ".bin");

    if((fp = fopen(fileName, "rb")) == NULL){
        /* if new user */
        info.win = 0;
        info.lose = 0;
    }else{
        /* user */
        fread(&info, sizeof(struct info), 1, fp);
        fclose(fp);
    }

    strcpy(info.name, name);


    return info; 
}


struct room* getRoom(struct client* clnt){
    struct room* r;
    r = (struct room*) clnt->room;
    return r;
}


/*

    client ary functions 

*/

int findClient(struct client clnt[], int clnt_num, int socket)
/*
    return client's idx
    if error then return -1
 */
{
    int i;
    for(i=0; i<clnt_num; i++){
        if(clnt[i].socket == socket)
            return i;
    }

    return -1;
}


struct client* addClient(struct client clnt[], int* clnt_num,  int socket, char* name){
    struct client* new_clnt;

    pthread_mutex_lock(&clnt_lock);
    new_clnt = &(clnt[*clnt_num]);
    *clnt_num += 1;
    pthread_mutex_unlock(&clnt_lock);

    new_clnt->socket = socket;
    new_clnt->info = getClientInfo(name);
    
    return new_clnt;
}


int removeClient(struct client clnt[], int* clnt_num, int socket){
    int i, clnt_idx;


    pthread_mutex_lock(&clnt_lock);
    clnt_idx = findClient(clnt, *clnt_num, socket);
    for(i=clnt_idx; i<*clnt_num-1; i++){
        clnt[i] = clnt[i+1];
    }
    *clnt_num -= 1;
    pthread_mutex_unlock(&clnt_lock);

    return *clnt_num;
}



void sendMsgToNotInTheRoom(struct client clnt[], int clnt_num, struct message msg){
    int i;

    for(i=0; i<clnt_num; i++){
        if(isInTheGamingRoom(&(clnt[i])) == FALSE){
            sendMessageUser(msg, &(clnt[i]));
        }
    }
}