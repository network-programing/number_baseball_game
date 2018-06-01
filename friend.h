#include "common.h"
#include "struct.h"
#include "clnt_info.h"



/*

addFriend

acceptFriend

friendList

chatFriend

inviteFriend

getFriend(char* name)

*/


int addFriend(struct info info[], int info_num, struct client* clnt, char* name);

int friendList(struct info info[], int info_num, struct client clnt_ary[], int clnt_num, struct client* clnt, char* listString);

int getFriend(struct info info[], int info_num, char* name);