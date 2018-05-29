#include "message.h"
#include <stdio.h>
#include <unistd.h>

void sendMessageUser(struct message msg, struct client* client){
    int length;
    length = write(client->socket, &msg, sizeof(struct message));
}