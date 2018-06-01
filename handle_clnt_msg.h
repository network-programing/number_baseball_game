#pragma once

#include "room.h"
#include "clnt_info.h"
#include "message.h"
#include "struct.h"


void handle_clnt_msg_in_waiting(struct info info[], int* info_num, struct room room[], int* room_num, struct client clnt_ary[], int* clnt_num, struct client** clnt, struct message msg, int* clnt_socket);

void handle_clnt_msg_in_gaming(struct info info[], int* info_num, struct room room[], int* room_num, struct client clnt_ary[], int* clnt_num, struct client** clnt, struct message msg, int* clnt_socket);
