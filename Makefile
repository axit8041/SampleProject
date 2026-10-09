# Makefile for the Sample Project

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGETS = hello_world bubble_sort

.PHONY: all clean test

all: $(TARGETS)

hello_world: hello_world.c
	$(CC) $(CFLAGS) -o $@ $^

bubble_sort: bubble_sort.c
	$(CC) $(CFLAGS) -o $@ $^

test: $(TARGETS)
	bash tests/test_hello_world.sh
	bash tests/test_bubble_sort.sh

clean:
	-rm -f $(TARGETS)
