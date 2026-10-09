# Makefile for the Hello World program

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = hello_world
SRC = hello_world.c

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TARGET)
	bash tests/test_hello_world.sh

clean:
	-rm -f $(TARGET)
