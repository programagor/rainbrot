CC=gcc
CFLAGS=-std=c99 -Isrc -Wall -pedantic -O3 -pthread -lm

all: bin/rainbrot-gen
	rm bin/*.o

bin/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

bin/rainbrot-gen: bin/main.o bin/arguments.o bin/list_tools.o bin/worker.o
	$(CC) $^ -o $@ $(CFLAGS)

install: bin/rainbrot-gen

uninstall:

clean:
	rm -rf bin/*
