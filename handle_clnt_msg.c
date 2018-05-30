#include "clnt_info.h"
#include "common.h"
#include "message.h"
#include "handle_clnt_msg.h"

#include <string.h>
#include <stdio.h>


/*  clinet is in the gaming room    */
void handle_clnt_msg_in_gaming_chatingMode(struct client* clnt, struct message msg){
    printf("msg is %s", msg.content);
    sendMessageToRoom(msg, (struct room*) clnt->room);
}


void handle_clnt_msg_in_gaming_selectMode(struct room room[], int* room_num, struct client* clnt, struct message msg){
    char option[40], buf[BUF_SIZE];
    int room_idx, room_id;
    struct room* clnt_room;

    printf("[%s] send %s\n", clnt->info.name, msg.content);

    sscanf(msg.content, "%s", option);

    if(strcmp(option, "quit") == 0){
        clnt_room = (struct room*) clnt->room;
        room_id = clnt_room->id;
        removeClientInRoom(clnt_room, clnt);

        if(room->clnt_num == 0)
        {
            removeRoom(room, room_num, room_id);
            printf("room[ id : %d ] is closed\n", room_id);
        }
    }else if(strcmp(option, "help") == 0){
        sendGamingRoomMenu(clnt);
    }
}


void handle_clnt_msg_in_gaming(struct room room[], int* room_num, struct client* clnt, struct message msg){
    struct message mode_error_msg = {"error", "mode is not matched\n"};

    if(strcmp(msg.mode, "select") == 0){
        handle_clnt_msg_in_gaming_selectMode(room, room_num, clnt, msg);
    }
    else if(strcmp(msg.mode, "chat") == 0){
        handle_clnt_msg_in_gaming_chatingMode(clnt, msg);
    }
    else{
        sendMessageUser(mode_error_msg, clnt);
    }
}





/*  clinet is in the wating room    */
void handle_clnt_msg_in_wating_chatingMode(){
    /* not yet */
}

void handle_clnt_msg_in_wating_selectMode(struct room room[], int* room_num, struct client* clnt, struct message msg){
    char option[40], buf[BUF_SIZE];
    struct message serv_msg, error_msg = {"error", "write right format\n"};
    int room_idx, room_id, num;

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

        sprintf(serv_msg.content, "[%s] is enter [%s] room!\n\n", clnt->info.name, room[room_idx].name);
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
    //  print option that client can choose in wating room
    else if(strcmp(option, "help") == 0){
        sendWaitingRoomMenu(clnt);
    }
    //  cannot find option
    else{
        sendMessageUser(error_msg, clnt);
    }
}


void handle_clnt_msg_in_waiting(struct room room[], int* room_num, struct client* clnt, struct message msg){
    struct message mode_error_msg = {"error", "mode is not matched\n"};

    if(strcmp(msg.mode, "select") == 0){
        handle_clnt_msg_in_wating_selectMode(room, room_num, clnt, msg);
    }
    else if(strcmp(msg.mode, "chat") == 0){
        handle_clnt_msg_in_wating_chatingMode();
    }
    else{
        sendMessageUser(mode_error_msg, clnt);
    }
}
