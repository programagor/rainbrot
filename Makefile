CC=gcc
CFLAGS=-std=c99 -Isrc -Wall -Wextra -pedantic -Ofast -pthread -lm

all: bin/rainbrot-gen
	rm bin/*.o

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

bin/rainbrot-gen: bin/main.o bin/arguments.o bin/list_tools.o bin/worker.o bin/functions.o
	$(CC) $^ -o $@ $(CFLAGS)

install: bin/rainbrot-gen

uninstall:

clean:
	rm -rf bin/*
