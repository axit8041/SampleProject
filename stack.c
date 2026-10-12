#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

stackStatus_e stack_create(stack_t **stack)
{
        if (stack == NULL) {
                return STACK_STATUS_NULL_POINTER;
        }

        stack_t *newStack = calloc(1, sizeof(*newStack));
        if (newStack == NULL) {
                return STACK_STATUS_MEMORY_ERROR;
        }

        newStack->topNode = NULL;
        newStack->nodeCount = 0;
        *stack = newStack;

        return STACK_STATUS_OK;
}

stackStatus_e stack_destroy(stack_t *stack)
{
        if (stack == NULL) {
                return STACK_STATUS_NULL_POINTER;
        }

        stackNode_t *currentNode = stack->topNode;
        stackStatus_e destroyStatus = STACK_STATUS_OK;

        while (currentNode != NULL) {
                stackNode_t *nextNode = currentNode->nextNode;
                free(currentNode);
                currentNode = nextNode;
        }

        free(stack);

        return destroyStatus;
}

stackStatus_e stack_push(stack_t *stack, int value)
{
        if (stack == NULL) {
                return STACK_STATUS_NULL_POINTER;
        }

        stackNode_t *newNode = malloc(sizeof(*newNode));
        if (newNode == NULL) {
                return STACK_STATUS_MEMORY_ERROR;
        }

        newNode->data = value;
        newNode->nextNode = stack->topNode;
        stack->topNode = newNode;
        stack->nodeCount++;

        return STACK_STATUS_OK;
}

stackStatus_e stack_pop(stack_t *stack, int *value)
{
        if ((stack == NULL) || (value == NULL)) {
                return STACK_STATUS_NULL_POINTER;
        }

        if (stack->topNode == NULL) {
                return STACK_STATUS_EMPTY;
        }

        stackNode_t *topNode = stack->topNode;
        *value = topNode->data;
        stack->topNode = topNode->nextNode;
        stack->nodeCount--;
        free(topNode);

        return STACK_STATUS_OK;
}

stackStatus_e stack_peek(const stack_t *stack, int *value)
{
        if ((stack == NULL) || (value == NULL)) {
                return STACK_STATUS_NULL_POINTER;
        }

        if (stack->topNode == NULL) {
                return STACK_STATUS_EMPTY;
        }

        *value = stack->topNode->data;

        return STACK_STATUS_OK;
}

stackStatus_e stack_size(const stack_t *stack, size_t *size)
{
        if ((stack == NULL) || (size == NULL)) {
                return STACK_STATUS_NULL_POINTER;
        }

        *size = stack->nodeCount;

        return STACK_STATUS_OK;
}

stackStatus_e stack_is_empty(const stack_t *stack, bool *isEmpty)
{
        if ((stack == NULL) || (isEmpty == NULL)) {
                return STACK_STATUS_NULL_POINTER;
        }

        if (stack->nodeCount == 0) {
                *isEmpty = true;
        } else {
                *isEmpty = false;
        }

        return STACK_STATUS_OK;
}
