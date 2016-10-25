CC=gcc
CFLAGS=-pthread -Wall -g

lab2b:lab2b.o SortedList.o
	$(CC) $(CFLAGS) lab2b.o SortedList.o -o lab2b

dist:
	tar -czf lab2b-204667709.tar.gz SortedList.c SortedList.h lab2b.c Makefile README first.png second.png
clean:
	rm -f lab2b.o SortedList.o lab2b lab2b-204667709.tar.gz
