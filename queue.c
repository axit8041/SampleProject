#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

queueStatus_e queue_create(queue_t **queue)
{
        if (queue == NULL) {
                return QUEUE_STATUS_NULL_POINTER;
        }

        queue_t *newQueue = calloc(1, sizeof(*newQueue));
        if (newQueue == NULL) {
                return QUEUE_STATUS_MEMORY_ERROR;
        }

        newQueue->frontNode = NULL;
        newQueue->rearNode = NULL;
        newQueue->nodeCount = 0;
        *queue = newQueue;

        return QUEUE_STATUS_OK;
}

queueStatus_e queue_destroy(queue_t *queue)
{
        if (queue == NULL) {
                return QUEUE_STATUS_NULL_POINTER;
        }

        queueNode_t *currentNode = queue->frontNode;
        queueStatus_e destroyStatus = QUEUE_STATUS_OK;

        while (currentNode != NULL) {
                queueNode_t *nextNode = currentNode->nextNode;
                free(currentNode);
                currentNode = nextNode;
        }

        free(queue);

        return destroyStatus;
}

queueStatus_e queue_enqueue(queue_t *queue, int value)
{
        if (queue == NULL) {
                return QUEUE_STATUS_NULL_POINTER;
        }

        queueNode_t *newNode = malloc(sizeof(*newNode));
        if (newNode == NULL) {
                return QUEUE_STATUS_MEMORY_ERROR;
        }

        newNode->data = value;
        newNode->nextNode = NULL;

        if (queue->rearNode == NULL) {
                queue->frontNode = newNode;
                queue->rearNode = newNode;
        } else {
                queue->rearNode->nextNode = newNode;
                queue->rearNode = newNode;
        }

        queue->nodeCount++;

        return QUEUE_STATUS_OK;
}

queueStatus_e queue_dequeue(queue_t *queue, int *value)
{
        if ((queue == NULL) || (value == NULL)) {
                return QUEUE_STATUS_NULL_POINTER;
        }

        if (queue->frontNode == NULL) {
                return QUEUE_STATUS_EMPTY;
        }

        queueNode_t *frontNode = queue->frontNode;
        *value = frontNode->data;
        queue->frontNode = frontNode->nextNode;

        if (queue->frontNode == NULL) {
                queue->rearNode = NULL;
        }

        queue->nodeCount--;
        free(frontNode);

        return QUEUE_STATUS_OK;
}

queueStatus_e queue_peek(const queue_t *queue, int *value)
{
        if ((queue == NULL) || (value == NULL)) {
                return QUEUE_STATUS_NULL_POINTER;
        }

        if (queue->frontNode == NULL) {
                return QUEUE_STATUS_EMPTY;
        }

        *value = queue->frontNode->data;

        return QUEUE_STATUS_OK;
}

queueStatus_e queue_size(const queue_t *queue, size_t *size)
{
        if ((queue == NULL) || (size == NULL)) {
                return QUEUE_STATUS_NULL_POINTER;
        }

        *size = queue->nodeCount;

        return QUEUE_STATUS_OK;
}

queueStatus_e queue_is_empty(const queue_t *queue, bool *isEmpty)
{
        if ((queue == NULL) || (isEmpty == NULL)) {
                return QUEUE_STATUS_NULL_POINTER;
        }

        if (queue->nodeCount == 0) {
                *isEmpty = true;
        } else {
                *isEmpty = false;
        }

        return QUEUE_STATUS_OK;
}
