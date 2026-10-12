# Makefile for the Sample Project

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGETS = $(patsubst %.c,%,$(wildcard *.c))

.PHONY: all clean test

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^

test: $(TARGETS)
	bash tests/test_hello_world.sh
	if test -f bubble_sort; then bash tests/test_bubble_sort.sh; fi
	bash tests/test_linear_search.sh

clean:
	-rm -f $(TARGETS)
