#pragma once

#include "common.h"
#include "room.h"
#include "struct.h"



/* clnt info functions */
void clnt_to_str(struct client* clnt, char str[]);

struct room* getRoom(struct client* clnt);


/* clnt info ary functions */
struct client* addClient(struct info info[], int* info_num, struct client clnt[], int* clnt_num, int socket, char* name);

int removeClient(int down[], struct client clnt[], int* clnt_num, int socket);

int findClient(struct client clnt[], int clnt_num, int socket);

int findClientWithName(struct client clnt[], int clnt_num, char* name);

void sendMsgToNotInTheRoom(struct client clnt[], int clnt_num, struct message msg);

void updateClientInfo(struct info info_ary[], int info_num, struct client* clnt, struct info info);


/* info function */
int readUserInfo(struct info info[], int* info_num);

int saveClientInfo(struct client* clnt);

int getClientInfo(struct info info[], int info_num, char* name);

int addInfo(struct info info_ary[], int* info_num, struct info info);

void infoToString(struct info info, char* buf);

int updateInfo(struct info info_ary[], int info_num, struct info info);