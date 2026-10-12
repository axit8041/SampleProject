/**
 * @file test_stack_queue.c
 * @brief Direct unit tests for the stack and queue APIs.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "queue.h"

static int totalFailures = 0;

static void check_condition(const char *testName, bool condition)
{
        if (!condition) {
                fprintf(stderr, "[FAIL] %s\n", testName);
                totalFailures++;
        } else {
                printf("[PASS] %s\n", testName);
        }
}

static void test_stack_operations(void)
{
        stack_t *stack = NULL;
        int value = 0;
        size_t size = 0;
        bool isEmpty = false;

        check_condition("stack_create returns OK",
                        stack_create(&stack) == STACK_STATUS_OK);
        check_condition("stack_create output pointer is not NULL",
                        stack != NULL);

        check_condition("stack_is_empty on new stack returns true",
                        (stack_is_empty(stack, &isEmpty) == STACK_STATUS_OK) &&
                        (isEmpty == true));
        check_condition("stack_size on new stack returns zero",
                        (stack_size(stack, &size) == STACK_STATUS_OK) &&
                        (size == 0));
        check_condition("stack_pop on empty stack returns EMPTY",
                        stack_pop(stack, &value) == STACK_STATUS_EMPTY);
        check_condition("stack_peek on empty stack returns EMPTY",
                        stack_peek(stack, &value) == STACK_STATUS_EMPTY);

        check_condition("stack_push 10 returns OK",
                        stack_push(stack, 10) == STACK_STATUS_OK);
        check_condition("stack_push 20 returns OK",
                        stack_push(stack, 20) == STACK_STATUS_OK);
        check_condition("stack_push 30 returns OK",
                        stack_push(stack, 30) == STACK_STATUS_OK);
        check_condition("stack_size after three pushes is three",
                        (stack_size(stack, &size) == STACK_STATUS_OK) &&
                        (size == 3));
        check_condition("stack_is_empty after pushes returns false",
                        (stack_is_empty(stack, &isEmpty) == STACK_STATUS_OK) &&
                        (isEmpty == false));

        check_condition("stack_peek returns top value 30",
                        (stack_peek(stack, &value) == STACK_STATUS_OK) &&
                        (value == 30));
        check_condition("stack_pop returns top value 30",
                        (stack_pop(stack, &value) == STACK_STATUS_OK) &&
                        (value == 30));
        check_condition("stack_pop returns next value 20",
                        (stack_pop(stack, &value) == STACK_STATUS_OK) &&
                        (value == 20));
        check_condition("stack_size after two pops is one",
                        (stack_size(stack, &size) == STACK_STATUS_OK) &&
                        (size == 1));

        check_condition("stack_destroy returns OK",
                        stack_destroy(stack) == STACK_STATUS_OK);

        check_condition("stack_create with NULL pointer returns NULL_POINTER",
                        stack_create(NULL) == STACK_STATUS_NULL_POINTER);
        check_condition("stack_push with NULL stack returns NULL_POINTER",
                        stack_push(NULL, 5) == STACK_STATUS_NULL_POINTER);
        check_condition("stack_destroy with NULL stack returns NULL_POINTER",
                        stack_destroy(NULL) == STACK_STATUS_NULL_POINTER);
}

static void test_queue_operations(void)
{
        queue_t *queue = NULL;
        int value = 0;
        size_t size = 0;
        bool isEmpty = false;

        check_condition("queue_create returns OK",
                        queue_create(&queue) == QUEUE_STATUS_OK);
        check_condition("queue_create output pointer is not NULL",
                        queue != NULL);

        check_condition("queue_is_empty on new queue returns true",
                        (queue_is_empty(queue, &isEmpty) == QUEUE_STATUS_OK) &&
                        (isEmpty == true));
        check_condition("queue_size on new queue returns zero",
                        (queue_size(queue, &size) == QUEUE_STATUS_OK) &&
                        (size == 0));
        check_condition("queue_dequeue on empty queue returns EMPTY",
                        queue_dequeue(queue, &value) == QUEUE_STATUS_EMPTY);
        check_condition("queue_peek on empty queue returns EMPTY",
                        queue_peek(queue, &value) == QUEUE_STATUS_EMPTY);

        check_condition("queue_enqueue 10 returns OK",
                        queue_enqueue(queue, 10) == QUEUE_STATUS_OK);
        check_condition("queue_enqueue 20 returns OK",
                        queue_enqueue(queue, 20) == QUEUE_STATUS_OK);
        check_condition("queue_enqueue 30 returns OK",
                        queue_enqueue(queue, 30) == QUEUE_STATUS_OK);
        check_condition("queue_size after three enqueues is three",
                        (queue_size(queue, &size) == QUEUE_STATUS_OK) &&
                        (size == 3));
        check_condition("queue_is_empty after enqueues returns false",
                        (queue_is_empty(queue, &isEmpty) == QUEUE_STATUS_OK) &&
                        (isEmpty == false));

        check_condition("queue_peek returns front value 10",
                        (queue_peek(queue, &value) == QUEUE_STATUS_OK) &&
                        (value == 10));
        check_condition("queue_dequeue returns front value 10",
                        (queue_dequeue(queue, &value) == QUEUE_STATUS_OK) &&
                        (value == 10));
        check_condition("queue_dequeue returns next value 20",
                        (queue_dequeue(queue, &value) == QUEUE_STATUS_OK) &&
                        (value == 20));
        check_condition("queue_size after two dequeues is one",
                        (queue_size(queue, &size) == QUEUE_STATUS_OK) &&
                        (size == 1));

        check_condition("queue_destroy returns OK",
                        queue_destroy(queue) == QUEUE_STATUS_OK);

        check_condition("queue_create with NULL pointer returns NULL_POINTER",
                        queue_create(NULL) == QUEUE_STATUS_NULL_POINTER);
        check_condition("queue_enqueue with NULL queue returns NULL_POINTER",
                        queue_enqueue(NULL, 5) == QUEUE_STATUS_NULL_POINTER);
        check_condition("queue_destroy with NULL queue returns NULL_POINTER",
                        queue_destroy(NULL) == QUEUE_STATUS_NULL_POINTER);
}

int main(void)
{
        printf("[TEST] Running stack unit tests...\n");
        test_stack_operations();
        printf("[TEST] Running queue unit tests...\n");
        test_queue_operations();

        if (totalFailures > 0) {
                fprintf(stderr, "[FAIL] %d test(s) failed.\n", totalFailures);
                return 1;
        }

        printf("[PASS] All stack and queue unit tests passed.\n");
        return 0;
}
