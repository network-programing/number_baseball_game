#include "clnt_info.h"
#include "struct.h"
#include "message.h"
#include "room.h"

#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>


pthread_mutex_t clnt_lock = PTHREAD_MUTEX_INITIALIZER;

struct info newInfo(char* name);
/*

    client functions

*/



void clnt_to_str(struct client *clnt, char str[]){
    int str_len;
    char buf[100];

    sprintf(str, "--------[%s] client's info-------\n", clnt->info.name);
    
    infoToString(clnt->info, buf);
    strcat(str, buf);
}


int set_nonblock_socket(int fd){
    int flags;
    if((flags = fcntl(fd, F_GETFL, 0)) == -1){
        return -1;
    }

    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return 0;
}


int saveClientInfo(struct client* clnt){
    FILE* fp;
    char fileName[100];
    int fd;
    
    /* file open */
    sprintf(fileName, "%s", clnt->info.name);
    strcat(fileName, ".bin");

    if((fp = fopen(fileName, "wb")) == NULL){
        return -1;
    }

    fd = fileno(fp);
    
    /*
    if(set_nonblock_socket(fd) == -1)
        return -1;
    */

    write(fd, &(clnt->info), sizeof(struct info));

    close(fd);

    return 0;
}



struct room* getRoom(struct client* clnt){
    struct room* r;
    r = (struct room*) clnt->room;
    return r;
}


/*

    client ary functions 

*/

int findClient(struct client clnt[], int clnt_num, int socket)
/*
    return client's idx
    if error then return -1
 */
{
    int i;
    for(i=0; i<clnt_num; i++){
        if(clnt[i].socket == socket)
            return i;
    }

    return -1;
}



int findClientWithName(struct client clnt[], int clnt_num, char* name){
    int i;
    for(i=0; i<clnt_num; i++){
        if(strcmp(clnt[i].info.name, name) == 0)
            return i;
    }

    return -1;
}



struct client* addClient(struct info info[], int* info_num, struct client clnt[], int* clnt_num,  int socket, char* name){
    struct client* new_clnt;
    int idx;

    pthread_mutex_lock(&clnt_lock);
    new_clnt = &(clnt[*clnt_num]);
    *clnt_num += 1;

    new_clnt->socket = socket;

    if((idx = getClientInfo(info, *info_num, name)) == -1){
        new_clnt->info = newInfo(name);
        addInfo(info, info_num, new_clnt->info);
    }else{
        new_clnt->info = info[idx];
    }
    pthread_mutex_unlock(&clnt_lock);

    return new_clnt;
}


int removeClient(int down[], struct client clnt[], int* clnt_num, int socket){
    int i, clnt_idx;


    pthread_mutex_lock(&clnt_lock);
    clnt_idx = findClient(clnt, *clnt_num, socket);

    for(i=0; i<*clnt_num; i++){
        if(i <= clnt_idx)
            down[i] = 1;
    }

    for(i=clnt_idx; i<*clnt_num-1; i++){
        clnt[i] = clnt[i+1];
    }
    *clnt_num -= 1;

    pthread_mutex_unlock(&clnt_lock);

    return *clnt_num;
}



void sendMsgToNotInTheRoom(struct client clnt[], int clnt_num, struct message msg){
    int i;

    for(i=0; i<clnt_num; i++){
        if(isInTheGamingRoom(&(clnt[i])) == FALSE){
            sendMessageUser(msg, &(clnt[i]));
        }
    }
}




void updateClientInfo(struct info info_ary[], int info_num, struct client* clnt, struct info info){
    int idx;

    idx = getClientInfo(info_ary, info_num, clnt->info.name);
    updateInfo(info_ary, idx, info);
    clnt->info = info;
    saveClientInfo(clnt);
}


/**/

int readFile(char * fileName, struct info* info){
    FILE* fp;

    if((fp = fopen(fileName, "r")) == NULL){
        return -1;
    }

    fread(info, sizeof(struct info), 1, fp);

    fclose(fp);
}

int readUserInfo(struct info info[], int* info_num){
    DIR *dir;
    struct dirent *ent;
    char buf[100], * ptr;  
    int i = 0;

    dir = opendir(".");
    if(dir == NULL) return -1;

    while((ent = readdir(dir)) != NULL){
        ptr = ent->d_name;
        while(*ptr != '.') ptr++;

        if(strcmp(ptr, ".bin") == 0){
            readFile(ent->d_name, &(info[i]));
            i++;
        }
    }
    closedir(dir);

    *info_num = i;

    return 0;
}


int addInfo(struct info info_ary[], int* info_num, struct info info){
    info_ary[*info_num] = info;
    *info_num += 1;
}

struct info newInfo(char* name){
    struct info info;

    info.win = 0;
    info.lose = 0;
    strcpy(info.name, name);
    info.f_list.num = 0;

    return info;
}



void infoToString(struct info info, char* buf){
    char buf2[100];

    sprintf(buf, "[name] : %s, [win] : %d, [lose] : %d", info.name, info.win, info.lose);
    
    if(info.win == 0)
        strcat(buf, ", [winning rate] : 0 %%");
    else{
        sprintf(buf2, ", [winning rate] : %.2lf %%", ((double)info.win/(info.win + info.lose) * 100 ));
        strcat(buf, buf2);
    }
}



int updateInfo(struct info info_ary[], int info_num, struct info info){
    info_ary[info_num] = info;
}

int getClientInfo(struct info info[], int info_num, char* name){
    int i;
    for(i=0; i<info_num; i++){
        if(strcmp(info[i].name, name) == 0)
            return i;
    }

    return -1; 
}
