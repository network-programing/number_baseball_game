#include "common.h"
#include "clnt.h"

#define MODE_SIZE 100

struct message{
    char mode[MODE_SIZE];
    char content[BUF_SIZE];
};


void sendMessageUser(struct message msg, struct client* client);