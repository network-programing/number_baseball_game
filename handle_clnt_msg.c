#include "clnt_info.h"
#include "common.h"
#include "message.h"
#include "handle_clnt_msg.h"
#include "friend.h"

#include <string.h>
#include <stdio.h>

int down[1000];


/*  clinet is in the gaming room    */
void handle_clnt_msg_in_gaming_chatingMode(struct client* clnt, struct message msg){
    printf("msg is %s", msg.content);
    sendMessageToRoom(msg, (struct room*) clnt->room);
}


void handle_clnt_msg_in_gaming_selectMode(struct info info[], int* info_num, struct client clnt_ary[], int* clnt_num, struct room room[], int* room_num, struct client* clnt, struct message msg){
    char option[40], buf[BUF_SIZE], chat_buf[BUF_SIZE];
    int room_idx, room_id, i, num;
    struct message serv_msg, error_msg = {"error", "write right format\n"};
    struct room* clnt_room;

    printf("[%s] send %s\n", clnt->info.name, msg.content);

    sscanf(msg.content, "%s", option);


    if(strcmp(option, "start") == 0){

    }
    else if(strcmp(option, "invite") == 0){

    }
    /* add friend */
    else if(strcmp(option, "add") == 0){
        if((num = sscanf(msg.content, "%s %s", option, buf)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        if((i = addFriend(info, *info_num, clnt,  buf)) == -1)
        {
            sprintf(serv_msg.content, "%s is not existed\n", buf);
            sendMessageUser(serv_msg, clnt);
            return;
        }else{
            i = findClientWithName(clnt_ary, *clnt_num, info[i].name);
            sprintf(serv_msg.content, "[%s] add you in friend list\nIf you want to add, then send 'add %s' in select mode\n", clnt->info.name, clnt->info.name);
            sendMessageUser(serv_msg, &(clnt_ary[i]));
        }

        sprintf(serv_msg.content, "add %s to friend list\n", buf);
        sendMessageUser(serv_msg, clnt);
    }
    /* send someone */
    else if(strcmp(option, "send") == 0){
        if((num = sscanf(msg.content, "%s %s %s", option, buf, chat_buf)) != 3){
            sendMessageUser(error_msg, clnt);
            return;
        }

        sprintf(serv_msg.content, "[%s] send message : %s\n", clnt->info.name, chat_buf);

        i = findClientWithName(clnt_ary, *clnt_num, buf);
        sendMessageUser(serv_msg, &(clnt_ary[i]));
    }
    /* list friends */
    else if(strcmp(option, "f_list") == 0){
        friendList(info, *info_num, clnt_ary, *clnt_num, clnt, serv_msg.content);
        sendMessageUser(serv_msg, clnt);
    }
    else if(strcmp(option, "help") == 0){
        sendGamingRoomMenu(clnt);
    }
    /* quit gaming room */
    else if(strcmp(option, "quit") == 0){
        clnt_room = (struct room*) clnt->room;
        room_id = clnt_room->id;

        sprintf(serv_msg.content, "[%s] is exit the [id : %d] [name : %s] room\n", clnt->info.name, room_id, clnt_room->name);
        sendMessageToRoom(serv_msg ,(struct room*)clnt->room);

        /*
            test
        */
        clnt->info.win ++;
        updateClientInfo(info, *info_num, clnt, clnt->info);


        /*
            test
        */

        removeClientInRoom(clnt_room, clnt);

        sendWaitingRoomMenu(clnt);

        if(clnt_room->clnt_num == 0)
        {
            removeRoom(room, room_num, room_id);
            printf("room[ id : %d ] is closed\n", room_id);
        }
    }
    //  cannot find option
    else{
        sendMessageUser(error_msg, clnt);
    }
}


void handle_clnt_msg_in_gaming(struct info info[], int* info_num, struct room room[], int* room_num, struct client clnt_ary[], int* clnt_num, struct client** clnt, struct message msg, int* clnt_socket){
    struct message mode_error_msg = {"error", "mode is not matched\n"};
    int i;
    
    i = findClient(clnt_ary, *clnt_num, *clnt_socket);
    *clnt = &(clnt_ary[i]);


    if(strcmp(msg.mode, "select") == 0){
        handle_clnt_msg_in_gaming_selectMode(info, info_num, clnt_ary, clnt_num, room, room_num, *clnt, msg);
    }
    else if(strcmp(msg.mode, "chat") == 0){
        handle_clnt_msg_in_gaming_chatingMode(*clnt, msg);
    }
    else{
        sendMessageUser(mode_error_msg, *clnt);
    }
}





/*  clinet is in the wating room    */

void handle_clnt_msg_in_wating_chatingMode(struct client clnt[], int *clnt_num, struct message msg){

    sendMsgToNotInTheRoom(clnt, *clnt_num, msg);
}

void handle_clnt_msg_in_wating_selectMode(struct info info[], int* info_num, struct room room[], int* room_num, struct client clnt_ary[], int* clnt_num, struct client* clnt, struct message msg){
    char option[40], buf[BUF_SIZE], chat_buf[BUF_SIZE];
    struct message serv_msg, error_msg = {"error", "write right format\n"};
    int room_idx, room_id, num, i;

    printf("[%s] send %s\n", clnt->info.name, msg.content);

    sscanf(msg.content, "%s", option);

    //  make gaming room
    if(strcmp(option, "create") == 0){
        if((num = sscanf(msg.content, "%s %s", option, buf)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        room_idx = makeRoom(room, room_num, buf);
        
        addClientToRoom(&(room[room_idx]), clnt);

        sprintf(serv_msg.content, "[%s] is enter [%s] room!\n\n", clnt->info.name, buf);
        sendMessageToRoom(serv_msg, (struct room*)clnt->room);
        
        sendGamingRoomMenu(clnt);

        printf("[num : %d] [name : %s] room is maked\n", room_idx, room[room_idx].name);
    }
    //  print gaming room list
    else if(strcmp(option, "list") == 0){
        roomList(room, *room_num, clnt, serv_msg.content);
        sendMessageUser(serv_msg, clnt);
    }
    //  enter gaming room
    else if(strcmp(option, "enter") == 0){
        if((num = sscanf(msg.content, "%s %d", option, &room_id)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        room_idx = findRoom(room, *room_num, room_id);
        if(room_idx == -1){
            sprintf(error_msg.content, "%d room not existed\n", room_id);
            sendMessageUser(error_msg, clnt);
            return;
        }

        if(addClientToRoom(&(room[room_idx]), clnt) == -1){
            sprintf(error_msg.content, "%d room is fulled\n", room_id);
            sendMessageUser(error_msg, clnt);
            return;
        }

        sprintf(serv_msg.content, "\n[%s] is enter [%s] room!\n\n", clnt->info.name, room[room_idx].name);
        sendMessageToRoom(serv_msg, (struct room*)clnt->room);

        sendGamingRoomMenu(clnt);

        printf("[clnt %s] is entered [num : %d] [name : %s] room\n", clnt->info.name, room[room_idx].id, room[room_idx].name);
    }
    //  info gaming room
    else if(strcmp(option, "info") == 0){
        if((num = sscanf(msg.content, "%s %d", option, &room_id)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        room_idx = findRoom(room, *room_num, room_id);
        if(room_idx == -1){
            sprintf(error_msg.content, "%d room not existed\n", room_id);
            sendMessageUser(error_msg, clnt);
            return;
        }

        specificRoomInfo(&(room[room_idx]), serv_msg.content);

        sendMessageUser(serv_msg, clnt);
    }
    /* add friend */
    else if(strcmp(option, "add") == 0){
        if((num = sscanf(msg.content, "%s %s", option, buf)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        if((i = addFriend(info, *info_num, clnt,  buf)) == -1)
        {
            sprintf(serv_msg.content, "%s is not existed\n", buf);
            sendMessageUser(serv_msg, clnt);
            return;
        }else{
            i = findClientWithName(clnt_ary, *clnt_num, info[i].name);
            sprintf(serv_msg.content, "[%s] add you in friend list\nIf you want to add, then send 'add %s' in select mode\n", clnt->info.name, clnt->info.name);
            sendMessageUser(serv_msg, &(clnt_ary[i]));
        }

        sprintf(serv_msg.content, "add %s to friend list\n", buf);
        sendMessageUser(serv_msg, clnt);
    }
    /* send someone */
    else if(strcmp(option, "send") == 0){
        if((num = sscanf(msg.content, "%s %s %s", option, buf, chat_buf)) != 3){
            sendMessageUser(error_msg, clnt);
            return;
        }

        sprintf(serv_msg.content, "[%s] send message : %s\n", clnt->info.name, chat_buf);

        if((i = findClientWithName(clnt_ary, *clnt_num, buf)) == -1){
            sprintf(serv_msg.content, "%s user is not connected\n", buf);
            sendMessageUser(serv_msg, clnt);
            return;
        }

        sendMessageUser(serv_msg, &(clnt_ary[i]));
    }
    /* list friends */
    else if(strcmp(option, "f_list") == 0){
        friendList(info, *info_num, clnt_ary, *clnt_num, clnt, serv_msg.content);
        sendMessageUser(serv_msg, clnt);
    }
    //  print option that client can choose in wating room
    else if(strcmp(option, "help") == 0){
        sendWaitingRoomMenu(clnt);
    }
    /* send rank */
    else if(strcmp(option, "rank") == 0){
        rankToString(info, *info_num, serv_msg.content);
        sendMessageUser(serv_msg, clnt);
    }else if(strcmp(option, "all") == 0){
        allConnectClient(clnt_ary, *clnt_num, serv_msg.content);
        sendMessageUser(serv_msg, clnt);
    }
    //  user exit
    else if(strcmp(option, "quit") == 0){

        /* send quit message to user */
        strcpy(serv_msg.mode, "quit");
        strcpy(serv_msg.content, "quit");
        sendMessageUser(serv_msg, clnt);

        strcpy(serv_msg.mode, "select");

        printf("[%s] quit server\n", clnt->info.name);
        
        /* save client info */
        updateClientInfo(info, *info_num, clnt, clnt->info);
        
        removeClient(down, clnt_ary, clnt_num, clnt->socket);
    }
    //  cannot find option
    else{
        sendMessageUser(error_msg, clnt);
    }
}


void handle_clnt_msg_in_waiting(struct info info[], int* info_num, struct room room[], int* room_num, struct client clnt_ary[], int* clnt_num, struct client** clnt, struct message msg, int* clnt_socket){
    struct message mode_error_msg = {"error", "mode is not matched\n"};
    int i, t;

    i = findClient(clnt_ary, *clnt_num, *clnt_socket);
    *clnt = &(clnt_ary[i]);

    if(strcmp(msg.mode, "select") == 0){
        handle_clnt_msg_in_wating_selectMode(info, info_num, room, room_num, clnt_ary, clnt_num , *clnt, msg);
    }
    else if(strcmp(msg.mode, "chat") == 0){
        handle_clnt_msg_in_wating_chatingMode(clnt_ary, clnt_num, msg);
    }
    else{
        sendMessageUser(mode_error_msg, *clnt);
    }
}
