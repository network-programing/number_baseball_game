#include "friend.h"

#include <stdio.h>
#include <string.h>


int insertFriend(struct friend* f_list, char* name){
    if(f_list->num >= FRIEND_MAX) return -1;

    strcpy(f_list->list[f_list->num], name);
    f_list->num += 1;

    return 0;
}



int addFriend(struct info info[], int info_num, struct client* clnt, char* name){
    int idx;

    idx = getFriend(info, info_num, name);

    if(idx == -1)   return -1;

    if(insertFriend(&(clnt->info.f_list), info[idx].name) == -1)
        return -1;

    return idx;
}



int friendList(struct info info[], int info_num, struct client clnt_ary[], int clnt_num, struct client* clnt, char* listString){
    int i, idx;
    struct client* friend;
    struct f_info;
    char buf[300];

    sprintf(listString, "<---- %s's friend ----->\n", clnt->info.name);

    for(i=0; i<clnt->info.f_list.num; i++){
        idx = getFriend(info, info_num, clnt->info.f_list.list[i]);

        if(idx == -1)
            return -1;

        infoToString(info[idx], buf);
        strcat(listString, buf);
        
        
        if((idx = findClientWithName(clnt_ary, clnt_num, info[idx].name)) == -1){
            strcat(listString, " [connect] : NO\n");
        }else{
            strcat(listString, " [connect] : YES\n");
        }
    }
}


int getFriend(struct info info[], int info_num, char* name){
    int i;

    for(i=0; i<info_num; i++){
        if(strcmp(info[i].name, name) == 0){
            return i;
        }
    }   

    return -1;
}



int sendMessageToFriends(struct client clnt_ary[], int clnt_num, struct client* clnt, struct message msg){
    int i, idx;
    struct friend f_list;
    struct client* friend;

    f_list = clnt->info.f_list;
    for(i=0; i<f_list.num; i++){
        idx = findClientWithName(clnt_ary, clnt_num, f_list.list[i]);
        
        if(idx != -1){
            friend = &(clnt_ary[idx]);
            sendMessageUser(msg, friend);
        } 
    }
}