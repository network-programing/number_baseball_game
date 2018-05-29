#include "clnt.h"
#include <string.h>

void clnt_to_str(struct client *clnt, char str[]){
    int str_len;
    char buf[50];

    sprintf(str, "%s client's info\n", clnt->name);
    
    sprintf(buf, "%d wins / %d loses\n", clnt->win, clnt->lose);
    strcat(str, buf);
}