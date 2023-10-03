all:
	gcc -Wall -c common.c
	gcc -Wall server.c common.o -o exeserver
	gcc -Wall client.c common.o -o execlient