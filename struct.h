#pragma once

#include "common.h"

#define MODE_SIZE 100

#define ROOM_IS_START 1001
#define ROOM_IS_NOT_START 1002
#define MAX_PERSON 3


/* client */
struct info{
    int win;
    int lose;
    char name[BUF_SIZE];
};


struct client{
    int socket;
    struct info info;
    void* room;
};


/* room */
struct room{
    int id;
    char name[BUF_SIZE];
    int status;
    void* clnt[MAX_PERSON];
    int clnt_num;
};






/* message */
struct message{
    char mode[MODE_SIZE];
    char content[BUF_SIZE];
};
