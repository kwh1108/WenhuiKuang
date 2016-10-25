CC = gcc
CFLAGS = -Wall -g
lab = lab3b

all: $(lab)

$(lab).o: $(lab).c
	$(CC) $(CFLAGS) -c -o $@ $<

$(lab): $(lab).o
	$(CC) $(CFLAGS) -o $@ $<

run: $(lab).o $(lab)
	./lab3b
clean:
	rm -f $(lab) $(lab).o $(lab)-204667709.tar.gz lab3b_check.txt

sources = Makefile README $(lab).c
dist:
	tar -czf $(lab)-204667709.tar.gz $(sources)
