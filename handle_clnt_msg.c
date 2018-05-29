#include "clnt.h"
#include "common.h"
#include "message.h"


/*  clinet is in the gaming room    */
void handle_clnt_msg_in_gaming_chatingMode(struct client* clnt, struct message msg){

    sendMessageToRoom(msg, clnt->room);
}


void handle_clnt_msg_in_gaming_selectMode(struct room room[], int* room_num, struct client* clnt, struct message msg){
    char option[40], buf[BUF_SIZE];
    int room_idx, room_id;

    if(strcmp(buf, "quit") == 0){
        room_id = clnt->room->id;
        removeClientInRoom(clnt->room, clnt);

        if(room->clnt_num == 0)
        {
            *room_num = removeRoom(room, room_num, room_id);
            printf("room[ id : %d ] is closed\n", room_id);
        }
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
        sendMessageUser(error_msg, clnt);
    }
}





/*  clinet is in the wating room    */
void handle_clnt_msg_in_wating_chatingMode(){
    /* not yet */
}

void handle_clnt_msg_in_wating_selectMode(struct room room[], int* room_num, struct client* clnt, struct message msg){
    char option[40], buf[BUF_SIZE];
    struct message serv_msg, error_msg = {"error", "write right format\n"};
    struct room* room;
    int room_idx, room_id, num;

    printf("handle %s's %s\n", clnt->name, msg.content);

    sscanf(msg.content, "%s", option);

    //  make gaming room
    if(strcmp(option, "create") == 0){
        if((num = sscanf(msg.content, "%s %s", option, buf)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        room_idx = makeRoom(room, room_num, buf);
        *room_num = room_idx;
        addClientToRoom(&(room[room_idx]), clnt);
    }
    //  print gaming room list
    else if(strcmp(option, "list") == 0){
        roomList(struct room[], int room_num, clnt, serv_msg.content);
        sendMessageUser(serv_msg, clnt);
    }
    //  enter gaming room
    else if(strcmp(option, "enter") == 0){
        if((num = sscanf(msg.content, "%s %d", option, room_id)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        room_idx = findRoom(room, room_num, room_id);
        addClientToRoom(&(room[room_idx]), clnt);
    }
    //  info gaming room
    else if(strcmp(option, "info") == 0){
        if((num = sscanf(msg.content, "%s %d", option, room_id)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        room_idx = findRoom(room, room_num, room_id);
        specificRoomInfo(&(room[room_idx]), serv_msg.content);
    }
    //  print option that client can choose in wating room
    else if(strcmp(option, "help") == 0){
        sendWaitingRoomMenu(clnt);
    }
    //  cannot find option
    else{
        snedMessageUser(error_msg, clnt);
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
