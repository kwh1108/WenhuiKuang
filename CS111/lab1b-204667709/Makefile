CC=gcc
CFLAGS=-pthread -Wall -lmcrypt -ldl

all:server client

server:server.o
	$(CC) $(CFLAGS) server.o -o server

client:client.o
	$(CC) $(CFLAGS) client.o -o client

dist:
	tar -czf lab1b-204667709.tar.gz server.c client.c Makefile README my.key

clean:
	rm -f server.o client.o client server lab1b-204667709.tar.gz
