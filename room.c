#include "room.h"
#include "message.h"
#include "clnt_info.h"

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t room_lock = PTHREAD_MUTEX_INITIALIZER;


/* 
    
    room functions
    
*/

int sendMessageToRoom(struct message msg, struct room* room){
    int i;
    struct client* clnt;

    for(i=0; i<room->clnt_num; i++){
        sendMessageUser(msg, room->clnt[i]);
    }
}



int addClientToRoom(struct room* room, struct client* clnt){
    if(room->clnt_num >= MAX_PERSON) return -1;

    room->clnt[room->clnt_num] = clnt;
    room->clnt_num += 1;
    clnt->room = room;

    return TRUE;
}

int findClientInRoom(struct room* room, struct client* clnt){
    int i;
    struct client* room_clnt;

    for(i=0; i<room->clnt_num; i++){
        room_clnt = (struct client*) room->clnt[i];
        if(room_clnt->socket == clnt->socket)
            return i;
    }

    return -1;
}


int removeClientInRoom(struct room* room, struct client* clnt){
    int idx, i;

    idx = findClientInRoom(room, clnt);
    if(idx == -1) return -1;

    for(i=idx; i<room->clnt_num-1; i++){
        room->clnt[i] = room->clnt[i+1];
    }

    room->clnt_num -= 1;
    clnt->room = NULL;
}




void statusToString(int status, char str[]){
    if(status == ROOM_IS_START)
        strcpy(str, "START");
    else if(status == ROOM_IS_NOT_START)
        strcpy(str, "NOT START");
}


void toStringRoom(struct room* room, char str[]){
    char status[20], buf[40];
    int len;

    sprintf(str, "[room name] : ");
    strcat(str, room->name);
    
    strcat(str, ", [status] : ");
    statusToString(room->status, status);
    strcat(str, status);
        
    sprintf(buf, ", [person] : %d / %d\n", room->clnt_num, MAX_PERSON);
    strcat(str, buf);
}




void sendWaitingRoomMenu(struct client* clnt){
	struct message msg;

	sprintf(msg.content, "\n<< Waiting Room Menu >>\n");
    strcat(msg.content, "[create] [name]: Create Room\n");
    strcat(msg.content, "[list]: List Room\n");	
	strcat(msg.content, "[enter] [room_number]: Enter Room\n");
	strcat(msg.content, "[info] [room_number] : Info Room\n");
    strcat(msg.content, "[add] [name] : add friend\n");
    strcat(msg.content, "[f_list] : list friend list\n");
    strcat(msg.content, "[send] [name] [content] : sned message\n");
    strcat(msg.content, "[rank] : ranking\n");
    strcat(msg.content, "[all] : list of conneted users\n");
	strcat(msg.content, "[help]: print menu\n");
	strcat(msg.content, "[quit]: Quit\n\n");

	sendMessageUser(msg, clnt);
}

void sendGamingRoomMenu(struct client* clnt){
	struct message msg;

	sprintf(msg.content, "\n<< Gaming Room Menu >>\n");	
    strcat(msg.content, "[start]: start game\n");
	strcat(msg.content, "[invite] [friend_name]: invite friend\n");
    strcat(msg.content, "[add] [name] : add friend\n");
    strcat(msg.content, "[send] [name] [content] : send message\n");
    strcat(msg.content, "[f_list] : list friend list\n");
	strcat(msg.content, "[help]: print menu\n");
	strcat(msg.content, "[quit]: Quit thist game room\n\n");

	sendMessageUser(msg, clnt);
}


int isInTheGamingRoom(struct client* clnt){
    struct room* room = (struct room*) clnt->room;

    if(room == NULL)
        return FALSE;
    else
        return TRUE;
}


int specificRoomInfo(struct room* room, char* str){
    int i;
    char buf[BUF_SIZE];

    toStringRoom(room, str);

    /* add clients info */
    for(i=0; i<room->clnt_num; i++){
        clnt_to_str(room->clnt[i], buf);
        strcat(str, buf);
        strcat(str, "\n");
    }

    strcat(str, "\n\n");
}

struct client* getClient(struct room* room, int index){
    if(index >= room->clnt_num) return NULL;

    return (struct client*)room->clnt[index];
}

/*

no use

struct room* makeRoomInstance(int id, char* name){
    struct room* newInstance;

    newInstance = (struct room*) malloc(sizeof(struct room));
    newInstance->clnt = (struct client*) malloc(sizeof(struct client*) * MAX_PERSON);

    newInstance->id = id;
    strcpy(newInstance->name, name);

    return newInstance
}

*/

/* 

    room ary functions 

*/

int removeRoom(struct room room[], int* room_num, int room_id){
/* if cannot find room then return -1
    if find room, then return new room_num */
    int idx, i, j;

    idx = findRoom(room, *room_num, room_id);
    if(idx == -1) return -1;

    pthread_mutex_lock(&room_lock);
    for(i=idx; i < *room_num-1; i++){

        (&(room[i+1]))->id = i;
        room[i] = room[i+1];

        for(j=0; j<room[i].clnt_num; j++){
            ((struct client*)room[i].clnt[j])->room = &(room[i]);
        }
    }
    *room_num -= 1;
    pthread_mutex_unlock(&room_lock);

    return *room_num;
}


int makeRoom(struct room room[], int* room_num, char* room_name){
    /* return new room_num */
    int idx = *room_num;

    pthread_mutex_lock(&room_lock);
    room[idx].id = idx;
    strcpy(room[idx].name, room_name);
    room[idx].clnt_num = 0;
    room[idx].status = ROOM_IS_NOT_START;
    *room_num += 1;
    pthread_mutex_unlock(&room_lock);

    return idx;
}


void roomList(struct room room[], int room_num, struct client* clnt, char* room_list){
    int i;
    char room_info[BUF_SIZE], buf[30];

    sprintf(room_list, "<------room------>\n");
    if(room_num == 0){
        strcat(room_list, "no room\n");
        return;
    }

    for(i=0; i<room_num; i++){
        sprintf(buf, "[number] : %d, ", i);
        strcat(room_list, buf);
        toStringRoom(&(room[i]), room_info);
        strcat(room_list, room_info);
        strcat(room_list, "\n");
    }
}


int findRoom(struct room room[], int room_num, int room_id)
/*
    return index of room
    if faild, then return -1
*/
{
    int i;

    for(int i=0; i<room_num; i++){
        if(room[i].id == room_id){
            return i;
        }
    }

    return -1;
}
