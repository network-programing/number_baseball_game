#pragma once

#include "common.h"
#include "room.h"
#include "struct.h"



/* clnt info functions */
void clnt_to_str(struct client* clnt, char str[]);
struct room* getRoom(struct client* clnt);


/* clnt info ary functions */
struct client* addClient(struct client clnt[], int* clnt_num, int socket, char* name);

int removeClient(struct client clnt[], int* clnt_num, int socket);

int findClient(struct client clnt[], int clnt_num, int socket);

void sendMsgToNotInTheRoom(struct client clnt[], int clnt_num, struct message msg);