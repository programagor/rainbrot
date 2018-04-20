CC=gcc
CFLAGS=-std=c99 -Isrc

all: bin/rainbrot-gen
	rm bin/*.o

bin/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

bin/rainbrot-gen: bin/main.o bin/arguments.o bin/csv_parse.o
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -rf bin/*
