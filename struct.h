#pragma once

#include "common.h"

#define MODE_SIZE 100

#define ROOM_IS_GAMING 1001
#define ROOM_IS_WAITING 1002
#define MAX_PERSON 3


/* room */
struct room{
    int id;
    char name[BUF_SIZE];
    int status;
    struct client* clnt[MAX_PERSON];
    int clnt_num;
};



/* client */
struct info{
    int win;
    int lose;
    char name[BUF_SIZE];
};


struct client{
    int socket;
    struct info info;
    struct room* room;
};




/* message */
struct message{
    char mode[MODE_SIZE];
    char content[BUF_SIZE];
};
