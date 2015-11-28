# Makefile for par-shell, version 1
# Sistemas Operativos, DEI/IST/ULisboa 2015-16

CFLAGS=-g -Wall -pedantic

all: par-shell fibonacci par-shell-terminal

par-shell: par-shell.o list.o sync.o commandlinereader.o utils.o
	gcc -o par-shell par-shell.o list.o sync.o commandlinereader.o utils.o -pthread

par-shell.o: par-shell.c list.h commandlinereader.h sync.h utils.h
	gcc $(CFLAGS) -c par-shell.c

par-shell-terminal: par-shell-terminal.o utils.o
	gcc -o par-shell-terminal par-shell-terminal.o utils.o

par-shell-terminal.o: par-shell-terminal.c utils.h
	gcc $(CFLAGS) -c par-shell-terminal.c

list.o: list.c list.h sync.h
	gcc $(CFLAGS) -c list.c

sync.o : sync.c sync.h list.h
	gcc $(CFLAGS) -c sync.c

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc $(CFLAGS) -c commandlinereader.c

utils.o: utils.c utils.h
	gcc $(CFLAGS) -c utils.c

fibonacci:
	gcc $(CFLAGS) -o fibonacci fibonacci.c

clean:
	rm *.o par-shell fibonacci
