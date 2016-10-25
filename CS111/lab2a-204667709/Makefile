CC=gcc
CFLAGS=-pthread -Wall

lab2a:lab2a.o
	$(CC) $(CFLAGS) lab2a.o -o lab2a

dist:
	tar -czf lab2a-204667709.tar.gz lab2a.c Makefile README first.png second.png 
clean:
	rm -f lab2a.o lab2a lab2a-204667709.tar.gz
