#include "common.h"
#include "room.h"


struct info{
    int win;
    int lose;
    int name[BUF_SIZE];
};


struct client{
    int socket;
    struct info info;
    struct room* room;
};


void clnt_to_str(struct client* clnt, char str[]);