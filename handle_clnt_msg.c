#include "clnt_info.h"
#include "common.h"
#include "message.h"
#include "handle_clnt_msg.h"
#include "friend.h"

#include <string.h>
#include <stdio.h>

int down[1000];
int invited[500];

int room_score[100][MAX_PERSON] = {0};


void memset_invited(){
    memset(invited, -1, sizeof(int)*500);
}

/*  clinet is in the gaming room    */
void handle_clnt_msg_in_gaming_chatingMode(struct client* clnt, struct message msg){
    sendMessageToRoom(msg, (struct room*) clnt->room);
}


void handle_clnt_msg_in_gaming_selectMode(struct info info[], int* info_num, struct client clnt_ary[], int* clnt_num, struct room room[], int* room_num, struct client* clnt, struct message msg){
    char option[40], buf[BUF_SIZE], chat_buf[BUF_SIZE];
    int room_idx, room_id, i, num;
    struct message serv_msg, error_msg = {"error", "write right format\n\n"};
    struct room* clnt_room;

    printf("[%s] send %s\n", clnt->info.name, msg.content);

    sscanf(msg.content, "%s", option);

    if(strcmp(option, "start") == 0){
        getRoom(clnt)->status = ROOM_IS_START;

        sprintf(serv_msg.mode, "start");
        sprintf(serv_msg.content, "game");
        sendMessageToRoom(serv_msg, clnt->room);
        
    }
    else if(strcmp(option, "invite") == 0){
        sprintf(serv_msg.mode, "common");

        if((num = sscanf(msg.content, "%s %s", option, buf)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        i = findClientWithName(clnt_ary, *clnt_num, buf);
        if(i == -1){
            sprintf(serv_msg.content, "%s is not connected\n", buf);
            sendMessageUser(serv_msg, clnt);
            return;
        }

        invited[i] = getRoom(clnt)->id;
        sprintf(serv_msg.content, "%s invite you\nif you want accpet, then type 'yes_invite'\n\n", clnt->info.name);
        sendMessageUser(serv_msg, &(clnt_ary[i]));
    }
    /* add friend */
    else if(strcmp(option, "f_add") == 0){
        sprintf(serv_msg.mode, "common");

        if((num = sscanf(msg.content, "%s %s", option, buf)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        /*  it's you    */
        if(strcmp(buf, clnt->info.name) == 0){
            sprintf(serv_msg.content, "it is you!\n\n");
            sendMessageUser(serv_msg, clnt);
            return;
        }


        /*  check  already friend  */
        for(i=0; i<clnt->info.f_list.num; i++){
            if(strcmp(buf, clnt->info.f_list.list[i]) == 0){
                sprintf(serv_msg.content, "%s is already your friend!\n\n", buf);
                sendMessageUser(serv_msg, clnt);
                return;
            }
        }

        /*  if not existed  */
        if((i = addFriend(info, *info_num, clnt,  buf)) == -1)
        {
            sprintf(serv_msg.content, "%s is not existed\n\n", buf);
            sendMessageUser(serv_msg, clnt);
            return;
        /*  add friend  */
        }else{
            i = findClientWithName(clnt_ary, *clnt_num, info[i].name);
            sprintf(serv_msg.content, "[%s] add you in friend list\nIf you want to add, then send 'f_add %s' in select mode\n\n", clnt->info.name, clnt->info.name);
            sendMessageUser(serv_msg, &(clnt_ary[i]));
        }

        sprintf(serv_msg.content, "add %s to friend list\n\n", buf);
        sendMessageUser(serv_msg, clnt);
    }
    /* send someone */
    else if(strcmp(option, "send") == 0){
        sprintf(serv_msg.mode, "common");

        if((num = sscanf(msg.content, "%s %s %s", option, buf, chat_buf)) != 3){
            sendMessageUser(error_msg, clnt);
            return;
        }

                strcpy(chat_buf, msg.content + strlen(option) + strlen(buf) + 2);

        if(strcmp(buf, clnt->info.name) == 0){
            sprintf(serv_msg.content, "It is you!\n\n");
            sendMessageUser(serv_msg, clnt);
            return;
        }

        sprintf(serv_msg.content, "[%s] send message : %s\n\n", clnt->info.name, chat_buf);

        if((i = findClientWithName(clnt_ary, *clnt_num, buf)) == -1){
            sprintf(serv_msg.content, "%s user is not connected\n\n", buf);
            sendMessageUser(serv_msg, clnt);
            return;
        }

        sendMessageUser(serv_msg, &(clnt_ary[i]));
    }
    /* list friends */
    else if(strcmp(option, "f_list") == 0){
        sprintf(serv_msg.mode, "common");

        friendList(info, *info_num, clnt_ary, *clnt_num, clnt, serv_msg.content);
        strcat(serv_msg.content, "\n");
        sendMessageUser(serv_msg, clnt);
    }
    /* send message to all friends */
    else if(strcmp(option, "f_send") == 0){
        sprintf(serv_msg.mode, "common");

        if((num = sscanf(msg.content, "%s %s", option, buf)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }


        sprintf(serv_msg.content, "[%s] send message : %s\n\n", clnt->info.name, msg.content + strlen(option) + 1);
        
        sendMessageToFriends(clnt_ary, *clnt_num, clnt, serv_msg);
    }
    else if(strcmp(option, "help") == 0){
        sprintf(serv_msg.mode, "common");

        sendGamingRoomMenu(clnt);
    }
    /* quit gaming room */
    else if(strcmp(option, "quit") == 0){
        sprintf(serv_msg.mode, "common");

        clnt_room = (struct room*) clnt->room;
        room_id = clnt_room->id;

        sprintf(serv_msg.content, "[%s] is exit the [id : %d] [name : %s] room\n\n", clnt->info.name, room_id, clnt_room->name);
        sendMessageToRoom(serv_msg ,(struct room*)clnt->room);

        /*
            test
        */
        //clnt->info.win ++;
        //updateClientInfo(info, *info_num, clnt, clnt->info);


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

    printf("serv message [%s] [%s]\n", serv_msg.mode, serv_msg.content);


}


void handle_in_gaming(struct room room[], int room_num, struct client *clnt, struct message msg, struct info info[], int info_num){
     int score, i, j, maxi, maxscore = -1, samecount = 0, lower = 98765432;
     struct message serv_msg;
     struct room* clntroom = clnt->room;
     struct client* winner, * losser;

    // single play
    if(clntroom->clnt_num == 1){
        sprintf(serv_msg.content, "game over\n\n");
        sendMessageUser(serv_msg, clnt);
        
        /*  room    status change*/
        getRoom(clnt)->status = ROOM_IS_NOT_START;

    }
    // multi play
    else{
        sscanf(msg.content, "%d", &score);
        sprintf(serv_msg.content,"gamer score is %d\ngame over\n\n", score);
        
        for(i=0; i<clntroom->clnt_num; i++){
            if(clntroom->clnt[i] != clnt){
                sendMessageUser(serv_msg, clntroom->clnt[i]);
            }else{
                j = findRoom(room, room_num, clntroom->id);
                printf("j is %d\n", j);
                room_score[j][i] = score;
                printf("i score is %d\n", room_score[j][i]);
            }
        }


        for(i=0; i<clntroom->clnt_num; i++){
            
            /*  if not finished */
            if(room_score[j][i] == 0)   return;

            if(maxscore < room_score[j][i]){
                maxscore = room_score[j][i];
                maxi = i;
            }
        }

        /*  room    status change*/
        getRoom(clnt)->status = ROOM_IS_NOT_START;


        if(room_score[j][0] == room_score[j][1]){
            /*  reset   */
            for(i=0; i<clntroom->clnt_num; i++){
                room_score[j][i] = 0;
            }
            return;
        }


        /*  update  winner   */
        winner = clntroom->clnt[maxi];
        winner->info.win ++;
        updateClientInfo(info, info_num, winner, winner->info);

        /*  update loser    */
        if(room_score[j][0] < room_score[j][1])
            losser = clntroom->clnt[0];
        else if(room_score[j][0] > room_score[j][1])
            losser = clntroom->clnt[1];

        printf("losser is %s\n", losser->info.name);

        losser->info.lose++;

        printf("update gogo\n");
        updateClientInfo(info, info_num, losser, losser->info);


        /*  reset   */
        for(i=0; i<clntroom->clnt_num; i++){
            room_score[j][i] = 0;
        }
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
    else if(strcmp(msg.mode, "game") == 0){
        handle_in_gaming(room, *room_num, *clnt, msg, info, *info_num);
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
    struct message serv_msg, error_msg = {"error", "write right format\n\n"};
    int room_idx, room_id, num, i;
    char* ptr;

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
        strcat(serv_msg.content, "\n");
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
            sprintf(error_msg.content, "%d room not existed\n\n", room_id);
            sendMessageUser(error_msg, clnt);
            return;
        }

        if(addClientToRoom(&(room[room_idx]), clnt) == -1){
            sprintf(error_msg.content, "%d room is fulled\n\n", room_id);
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
            sprintf(error_msg.content, "%d room not existed\n\n", room_id);
            sendMessageUser(error_msg, clnt);
            return;
        }

        specificRoomInfo(&(room[room_idx]), serv_msg.content);
        strcat(serv_msg.content, "\n");

        sendMessageUser(serv_msg, clnt);
    }
    /* add friend */
    else if(strcmp(option, "f_add") == 0){
        if((num = sscanf(msg.content, "%s %s", option, buf)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }


        /*  it's you    */
        if(strcmp(buf, clnt->info.name) == 0){
            sprintf(serv_msg.content, "it is you!\n\n");
            sendMessageUser(serv_msg, clnt);
            return;
        }


        /*  check  already friend  */
        for(i=0; i<clnt->info.f_list.num; i++){
            if(strcmp(buf, clnt->info.f_list.list[i]) == 0){
                sprintf(serv_msg.content, "%s is already your friend!\n\n", buf);
                sendMessageUser(serv_msg, clnt);
                return;
            }
        }


        if((i = addFriend(info, *info_num, clnt,  buf)) == -1)
        {
            sprintf(serv_msg.content, "%s is not existed\n\n", buf);
            sendMessageUser(serv_msg, clnt);
            return;
        }else{
            i = findClientWithName(clnt_ary, *clnt_num, info[i].name);
            sprintf(serv_msg.content, "[%s] add you in friend list\nIf you want to add, then send 'f_add %s' in select mode\n\n", clnt->info.name, clnt->info.name);
            sendMessageUser(serv_msg, &(clnt_ary[i]));
        }

        sprintf(serv_msg.content, "add %s to friend list\n\n", buf);
        sendMessageUser(serv_msg, clnt);
    }
    /* send someone */
    else if(strcmp(option, "send") == 0){
        if((num = sscanf(msg.content, "%s %s %s", option, buf, chat_buf)) != 3){
            sendMessageUser(error_msg, clnt);
            return;
        }

        strcpy(chat_buf, msg.content + strlen(option) + strlen(buf) + 2);

        if(strcmp(buf, clnt->info.name) == 0){
            sprintf(serv_msg.content, "It is you!\n\n");
            sendMessageUser(serv_msg, clnt);
            return;
        }

        sprintf(serv_msg.content, "[%s] send message : %s\n\n", clnt->info.name, chat_buf);

        if((i = findClientWithName(clnt_ary, *clnt_num, buf)) == -1){
            sprintf(serv_msg.content, "%s user is not connected\n\n", buf);
            sendMessageUser(serv_msg, clnt);
            return;
        }

        sendMessageUser(serv_msg, &(clnt_ary[i]));
    }
    /* list friends */
    else if(strcmp(option, "f_list") == 0){
        friendList(info, *info_num, clnt_ary, *clnt_num, clnt, serv_msg.content);
        strcat(serv_msg.content, "\n");
        sendMessageUser(serv_msg, clnt);
    }
    /* send message to all friends */
    else if(strcmp(option, "f_send") == 0){

        if((num = sscanf(msg.content, "%s %s", option, buf)) != 2){
            sendMessageUser(error_msg, clnt);
            return;
        }

        sprintf(serv_msg.content, "[%s] send message : %s\n\n", clnt->info.name, msg.content + strlen(option) + 1);

        sendMessageToFriends(clnt_ary, *clnt_num, clnt, serv_msg);
    }
    //  print option that client can choose in wating room
    else if(strcmp(option, "help") == 0){
        sendWaitingRoomMenu(clnt);
    }
    /* send rank */
    else if(strcmp(option, "rank") == 0){
        rankToString(info, *info_num, serv_msg.content);
        strcat(serv_msg.content, "\n");
        sendMessageUser(serv_msg, clnt);
    }else if(strcmp(option, "all") == 0){
        allConnectClient(clnt_ary, *clnt_num, serv_msg.content);
        strcat(serv_msg.content, "\n");
        sendMessageUser(serv_msg, clnt);
    }
    else if(strcmp(option, "yes_invite") == 0){
        i = findClient(clnt_ary, *clnt_num, clnt->socket);
        if(invited[i] >= 0){

            room_idx = findRoom(room, *room_num, invited[i]);
            invited[i] = -1;

            if(room_idx == -1){
                sprintf(error_msg.content, "%d room not existed\n\n", room_id);
                sendMessageUser(error_msg, clnt);
                return;
            }

            if(addClientToRoom(&(room[room_idx]), clnt) == -1){
                sprintf(error_msg.content, "%d room is fulled\n\n", room_id);
                sendMessageUser(error_msg, clnt);
                return;
            }

            sprintf(serv_msg.content, "\n[%s] is enter [%s] room!\n\n", clnt->info.name, room[room_idx].name);
            sendMessageToRoom(serv_msg, (struct room*)clnt->room);

            sendGamingRoomMenu(clnt);

            printf("[clnt %s] is entered [num : %d] [name : %s] room\n", clnt->info.name, room[room_idx].id, room[room_idx].name);
        }
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
