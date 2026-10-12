# Makefile for the Sample Project

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
GST_CFLAGS = $(shell pkg-config --cflags gstreamer-1.0)
GST_LIBS = $(shell pkg-config --libs gstreamer-1.0)
TARGETS = hello_world bubble_sort linear_search gstreamer_app

.PHONY: all clean test

all: $(TARGETS)

hello_world: hello_world.c
	$(CC) $(CFLAGS) -o $@ $^

bubble_sort: bubble_sort.c
	$(CC) $(CFLAGS) -o $@ $^

linear_search: linear_search.c
	$(CC) $(CFLAGS) -o $@ $^

gstreamer_app: gstreamer_app.c
	$(CC) $(CFLAGS) $(GST_CFLAGS) -o $@ $^ $(GST_LIBS)

test: $(TARGETS)
	bash tests/test_hello_world.sh
	bash tests/test_bubble_sort.sh
	bash tests/test_linear_search.sh
	bash tests/test_gstreamer_app.sh

clean:
	-rm -f $(TARGETS)
