CC=gcc
CFLAGS=-pthread -Wall -g -pg

lab2c:lab2c.o SortedList.o
	$(CC) $(CFLAGS) lab2c.o SortedList.o -o lab2c

dist:
	tar -czf lab2c-204667709.tar.gz SortedList.c SortedList.h lab2c.c Makefile README first.png
clean:
	rm -f lab2c.o SortedList.o lab2c lab2c-204667709.tar.gz
