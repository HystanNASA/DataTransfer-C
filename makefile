all: server.c client.c
	gcc server.c -o server
	gcc client.c -o client

s: server.c
	gcc server.c -o server

c: client.c
	gcc client.c -o client