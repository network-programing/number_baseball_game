#pragma once

#include "common.h"
#include "message.h"
#include "clnt_info.h"
#include "struct.h"



/* room functions */
int sendMessageToRoom(struct message msg, struct room* room);

int addClientToRoom(struct room* room, struct client* clnt);

int removeClientInRoom(struct room* room, struct client* clnt);

void sendWaitingRoomMenu(struct client* clnt);

void sendGamingRoomMenu(struct client* clnt);

int isInTheGamingRoom(struct client* clnt);

int specificRoomInfo(struct room* room, char* str);

struct client* getClient(struct room* room, int index);


/* room ary functions */
int removeRoom(struct room room[], int* room_num, int room_id);

int makeRoom(struct room room[], int* room_num, char* room_name);

void roomList(struct room room[], int room_num, struct client* clnt, char* room_list);

int findRoom(struct room room[], int room_num, int room_id);
