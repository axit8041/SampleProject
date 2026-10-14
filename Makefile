# Makefile for the Sample Project

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -fPIC
LDFLAGS =
AR = ar
ARFLAGS = rcs
GST_CFLAGS = $(shell pkg-config --cflags gstreamer-1.0)
GST_LIBS = $(shell pkg-config --libs gstreamer-1.0)
TARGETS = hello_world bubble_sort linear_search gstreamer_app stack_queue_app string_concat factorial
LIBRARIES = libdatastructures.a libdatastructures.so

.PHONY: all clean test

all: $(TARGETS) $(LIBRARIES)

hello_world: hello_world.c
	$(CC) $(CFLAGS) -o $@ $^

bubble_sort: bubble_sort.c
	$(CC) $(CFLAGS) -o $@ $^

linear_search: linear_search.c
	$(CC) $(CFLAGS) -o $@ $^

gstreamer_app: gstreamer_app.c
	$(CC) $(CFLAGS) $(GST_CFLAGS) -o $@ $^ $(GST_LIBS)

string_concat: string_concat.c
	$(CC) $(CFLAGS) -o $@ $^

factorial: factorial.c
	$(CC) $(CFLAGS) -o $@ $^

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c $< -o $@

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c $< -o $@

libdatastructures.a: stack.o queue.o
	$(AR) $(ARFLAGS) $@ $^

libdatastructures.so: stack.o queue.o
	$(CC) -shared -o $@ $^

stack_queue_app: stack_queue_app.c libdatastructures.a
	$(CC) $(CFLAGS) -o $@ $< libdatastructures.a

test: all
	bash tests/test_hello_world.sh
	bash tests/test_bubble_sort.sh
	bash tests/test_linear_search.sh
	bash tests/test_gstreamer_app.sh
	bash tests/test_stack_queue_app.sh
	bash tests/test_string_concat.sh
	bash tests/test_factorial.sh

clean:
	-rm -f $(TARGETS) $(LIBRARIES) *.o tests/test_stack_queue
