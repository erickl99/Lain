CC=gcc
CFLAGS=-Iinclude
exec = lainc
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -Wall -I./include

$(exec): $(objects)
	gcc $(objects) $(flags) -o build/$(exec)

%.o: %.c %.h
	@gcc -c $(flags) $< -o $@

.PHONY: clean
clean:
	@rm src/*.o build/*
