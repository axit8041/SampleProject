# Makefile for the Hello World program

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = hello_world
SRC = hello_world.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	-rm -f $(TARGET)
