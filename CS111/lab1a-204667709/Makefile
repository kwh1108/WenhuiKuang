CC=gcc
CFLAGS=-pthread -Wall

lab1a: lab1a.o
	$(CC) $(CFLAGS)  lab1a.o -o lab1a

dist:
	tar -czf lab1a-204667709.tar.gz lab1a.c Makefile README
clean:
	rm -f lab1a.o lab1a lab1a-204667709.tar.gz
