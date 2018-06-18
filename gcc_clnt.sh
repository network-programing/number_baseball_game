gcc -c -o snake.o snake.c -lncurses
gcc -c -o clnt.o clnt.c -lpthread -lncurses
gcc -o clnt.exe snake.o clnt.o -lncurses -lpthread
