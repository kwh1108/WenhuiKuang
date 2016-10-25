CC=gcc

lab0:lab0.c
	$(CC) -g lab0.c -o lab0
check:
	echo "asdasd" > file1
	./lab0 --input=file1 --output=file2
	diff -u file1 file2 || exit
dist:
	tar -czf lab0-204667709.tar.gz lab0.c Makefile README s1.png s2.png
clean:
	rm -f file1 file2 lab0.o lab0 lab0-204667709.tar.gz
