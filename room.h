#include "common.h"
#include "message.h"
#include "clnt.h"

#define ROOM_IS_GAMING 1001
#define ROOM_IS_WAITING 1002
#define MAX_PERSON 3


struct room{
    int id;
    char name[BUF_SIZE];
    int status;
    struct client* clnt[MAX_PERSON];
    int clnt_num;
};


int sendMessageToRoom(struct message msg, struct room* room);

int removeRoom(struct room[], int room_num, int room_id);

int addClientToRoom(struct room* room, struct client* clnt);

int removeClientInRoom(struct room* room, struct client* clnt);

int makeRoom(struct room[], int room_num, char* room_name);

void roomList(struct room[], int room_num, struct client* clnt, char* room_list)

void sendWaitingRoomMenu(struct client* clnt);

void sendGamingRoomMenu(struct client* clnt);

int isInTheGamingRoom(struct client* clnt);

int specificRoomInfo(struct room* room, char* str);

int findRoom(struct room[], int room_num, int room_id);