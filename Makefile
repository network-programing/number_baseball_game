serv_exe : clnt_info.o handle_clnt_msg.o message.o room.o serv_func.o serv.o
	gcc -o serv_exe.o clnt_info.o handle_clnt_msg.o message.o room.o serv_func.o serv.o -lpthread


clnt_info.o : clnt_info.c clnt_info.h
	gcc -c clnt_info.c -o clnt_info.o -lpthread


handle_clnt_msg.o : handle_clnt_msg.c common.h message.o
	gcc -c -o handle_clnt_msg.o handle_clnt_msg.c


message.o : message.c message.h
	gcc -c -o message.o message.c


room.o : room.c room.h message.o
	gcc -c room.c -o room.o -lpthread


serv_func.o : serv_func.c serv_func.h
	gcc -c -o serv_func.o serv_func.c


serv.o : serv.c serv_func.o room.o message.o handle_clnt_msg.o clnt_info.o
	gcc -c serv.c -o serv.o -lpthread


clnt.o : clnt.c message.o
	gcc -o clnt.o clnt.c


clean :
	rm *.o serv_exe