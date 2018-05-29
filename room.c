#include "room.h"
#include "message.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>

pthread_mutex_t room_lock = PTHREAD_MUTEX_INITIALIZER;


/* 
    
    room functions
    
*/

int sendMessageToRoom(struct message msg, struct room* room){
    int i;
    for(i=0; i<room->clnt_num; i++){
        sendMessageUser(msg, room->clnt[i]);
    }
}



int addClientToRoom(struct room* room, struct client* clnt){
    if(room->clnt_num >= MAX_PERSON) return -1;

    room->clnt[room->clnt_num] = clnt;
    room->clnt_num += 1;
    clnt->room->id = room->id;

    return TRUE;
}

int findClientInRoom(struct room* room, struct client* clnt){
    int i;

    for(i=0; i<room->clnt_num; i++){
        if(room->clnt[i]->socket == clnt->socket)
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
    if(status == ROOM_IS_GAMING)
        strcpy(str, "Gaming");
    else if(status == ROOM_IS_WAITING)
        strcpy(str, "Wating");
}


void toStringRoom(struct room* room, char str[]){
    char status[20], buf[40];
    int len;

    sprintf(str, "[room name] : ");
    strcat(str, room->name);
    
    strcat(str, ", [status] : ");
    statusToString(room->status, status);
    strcat(str, status);
        
    sprintf(buf, "%d / %d\n", room->clnt_num, MAX_PERSON);
    strcat(str, buf);
}




void sendWaitingRoomMenu(struct client* clnt){
	struct message msg;

	sprintf(msg.content, "<< Waiting Room Menu:>>\n[create] [name]: Create Room\n[list]: List Room\n");	
	strcat(msg.content, "[enter] [room_number]: Enter Room\n");
	strcat(msg.content, "[info] [room_number] : Info Room\n");
	strcat(msg.content, "[help]: print menu\n");
	strcat(msg.content, "[quit]: Quit\n");

	sendMessageUser(msg, clnt);
}

void sendGamingRoomMenu(struct client* clnt){
	struct message msg;

	sprintf(msg.content, "<< Waiting Room Menu: >>\n");	
	strcat(msg.content, "[stand]: Stand\n");
	strcat(msg.content,  "[hit]: Hit\n");
	strcat(msg.content, "[betting] [money] : Betting money\n");
	strcat(msg.content, "[help]: print menu\n");
	strcat(msg.content, "[quit]: Quit thist game room\n");

	sendMessageUser(msg, clnt);

}


int isInTheGamingRoom(struct client* clnt){
    if(clnt->room == NULL)
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
    }
}




/* 

    room ary functions 

*/

int removeRoom(struct room room[], int* room_num, int room_id){
/* if cannot find room then return -1
    if find room, then return new room_num */
    int idx, i;

    idx = findRoom(room, *room_num, room_id);
    if(idx == -1) return -1;

    pthread_mutex_lock(&room_lock);
    for(i=idx; i < *room_num-1; i++){
        room[i] = room[i+1];
    }
    *room_num -= 1;
    pthread_mutex_unlock(&room_lock);

    return *room_num;
}


int makeRoom(struct room room[], int* room_num, char* room_name){
    /* return new room_num */

    pthread_mutex_lock(&room_lock);
    (&room[*room_num])->id = *room_num + 1;
    strcpy((&(room[*room_num]))->name, room_name);
    *room_num += 1;
    pthread_mutex_unlock(&room_lock);

    return *room_num;
}


void roomList(struct room room[], int room_num, struct client* clnt, char* room_list){
    int i;
    char room_info[BUF_SIZE];

    sprintf(room_list, "<------room------>");
    
    for(i=0; i<room_num; i++){
        toStringRoom(&(room[i]), room_info);
        strcat(room_list, room_info);
    }
}


int findRoom(struct room room[], int room_num, int room_id){
    int i;

    for(int i=0; i<room_num; i++){
        if(room[i].id == room_id){
            return i;
        }
    }

    return -1;
}
