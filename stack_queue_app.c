#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "queue.h"

/**
 * @brief Entry point for the Stack and Queue demonstration application.
 *
 * Reads integers from standard input until EOF. Odd values are pushed onto
 * a stack; even values are enqueued into a queue. The program then prints
 * the top of the stack and the front of the queue along with their sizes.
 *
 * Input format: space or newline separated integers, terminated by EOF.
 *
 * @return 0 on success, non-zero on failure.
 */
int main(void)
{
        stack_t *stack = NULL;
        queue_t *queue = NULL;
        int currentValue = 0;
        int programStatus = 0;
        int bytesWritten = 0;

        stackStatus_e stackCreateStatus = stack_create(&stack);
        if (stackCreateStatus != STACK_STATUS_OK) {
                fprintf(stderr, "Failed to create stack\n");
                return 1;
        }

        queueStatus_e queueCreateStatus = queue_create(&queue);
        if (queueCreateStatus != QUEUE_STATUS_OK) {
                fprintf(stderr, "Failed to create queue\n");
                stack_destroy(stack);
                return 1;
        }

        while (scanf("%d", &currentValue) == 1) {
                if ((currentValue % 2) != 0) {
                        stackStatus_e pushStatus = stack_push(stack, currentValue);
                        if (pushStatus != STACK_STATUS_OK) {
                                fprintf(stderr, "Failed to push value %d\n", currentValue);
                                programStatus = 1;
                                goto cleanup;
                        }
                } else {
                        queueStatus_e enqueueStatus = queue_enqueue(queue, currentValue);
                        if (enqueueStatus != QUEUE_STATUS_OK) {
                                fprintf(stderr, "Failed to enqueue value %d\n", currentValue);
                                programStatus = 1;
                                goto cleanup;
                        }
                }
        }

        bool stackIsEmpty = false;
        stackStatus_e emptyStatus = stack_is_empty(stack, &stackIsEmpty);
        if (emptyStatus != STACK_STATUS_OK) {
                fprintf(stderr, "Failed to check stack empty state\n");
                programStatus = 1;
                goto cleanup;
        }

        if (stackIsEmpty) {
                bytesWritten = printf("Stack: empty\n");
                if (bytesWritten < 0) {
                        programStatus = 1;
                        goto cleanup;
                }
        } else {
                int stackTop = 0;
                size_t stackSize = 0;
                stackStatus_e peekStatus = stack_peek(stack, &stackTop);
                if (peekStatus != STACK_STATUS_OK) {
                        fprintf(stderr, "Failed to peek stack\n");
                        programStatus = 1;
                        goto cleanup;
                }
                stackStatus_e sizeStatus = stack_size(stack, &stackSize);
                if (sizeStatus != STACK_STATUS_OK) {
                        fprintf(stderr, "Failed to get stack size\n");
                        programStatus = 1;
                        goto cleanup;
                }
                bytesWritten = printf("Stack: top=%d size=%zu\n", stackTop, stackSize);
                if (bytesWritten < 0) {
                        programStatus = 1;
                        goto cleanup;
                }
        }

        bool queueIsEmpty = false;
        queueStatus_e queueEmptyStatus = queue_is_empty(queue, &queueIsEmpty);
        if (queueEmptyStatus != QUEUE_STATUS_OK) {
                fprintf(stderr, "Failed to check queue empty state\n");
                programStatus = 1;
                goto cleanup;
        }

        if (queueIsEmpty) {
                bytesWritten = printf("Queue: empty\n");
                if (bytesWritten < 0) {
                        programStatus = 1;
                        goto cleanup;
                }
        } else {
                int queueFront = 0;
                size_t queueSize = 0;
                queueStatus_e peekStatus = queue_peek(queue, &queueFront);
                if (peekStatus != QUEUE_STATUS_OK) {
                        fprintf(stderr, "Failed to peek queue\n");
                        programStatus = 1;
                        goto cleanup;
                }
                queueStatus_e sizeStatus = queue_size(queue, &queueSize);
                if (sizeStatus != QUEUE_STATUS_OK) {
                        fprintf(stderr, "Failed to get queue size\n");
                        programStatus = 1;
                        goto cleanup;
                }
                bytesWritten = printf("Queue: front=%d size=%zu\n", queueFront, queueSize);
                if (bytesWritten < 0) {
                        programStatus = 1;
                        goto cleanup;
                }
        }

cleanup:
        if (stack_destroy(stack) != STACK_STATUS_OK) {
                programStatus = 1;
        }
        if (queue_destroy(queue) != QUEUE_STATUS_OK) {
                programStatus = 1;
        }

        return programStatus;
}
