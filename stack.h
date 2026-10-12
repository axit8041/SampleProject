#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Status codes returned by the stack API.
 */
typedef enum {
        STACK_STATUS_OK = 0,              /**< Operation completed successfully. */
        STACK_STATUS_NULL_POINTER = 1,      /**< A required pointer argument was NULL. */
        STACK_STATUS_EMPTY = 2,             /**< The stack contains no elements. */
        STACK_STATUS_MEMORY_ERROR = 3       /**< Memory allocation failed. */
} stackStatus_e;

/**
 * @brief Internal node of the stack linked list.
 */
typedef struct stackNode {
        int data;                    /**< Value stored in the node. */
        struct stackNode *nextNode;  /**< Pointer to the next node below this one. */
} stackNode_t;

/**
 * @brief Stack container implemented as a linked list.
 */
typedef struct {
        stackNode_t *topNode;        /**< Node at the top of the stack. */
        size_t nodeCount;            /**< Number of elements currently stored. */
} stack_t;

/**
 * @brief Allocate and initialize a new stack.
 *
 * @param stack Output pointer that receives the newly created stack.
 *              Must not be NULL.
 *
 * @return STACK_STATUS_OK on success, STACK_STATUS_NULL_POINTER if stack is NULL,
 *         or STACK_STATUS_MEMORY_ERROR if allocation fails.
 */
stackStatus_e stack_create(stack_t **stack);

/**
 * @brief Destroy a stack and free all associated memory.
 *
 * @param stack Pointer to the stack to destroy. Passing NULL is safe and
 *              returns STACK_STATUS_NULL_POINTER.
 *
 * @return STACK_STATUS_OK on success or STACK_STATUS_NULL_POINTER if stack is NULL.
 */
stackStatus_e stack_destroy(stack_t *stack);

/**
 * @brief Push a value onto the top of the stack.
 *
 * @param stack Pointer to the stack. Must not be NULL.
 * @param value Integer value to push.
 *
 * @return STACK_STATUS_OK on success, STACK_STATUS_NULL_POINTER if stack is NULL,
 *         or STACK_STATUS_MEMORY_ERROR if a new node cannot be allocated.
 */
stackStatus_e stack_push(stack_t *stack, int value);

/**
 * @brief Pop the top value from the stack.
 *
 * @param stack Pointer to the stack. Must not be NULL.
 * @param value Output parameter that receives the popped value. Must not be NULL.
 *
 * @return STACK_STATUS_OK on success, STACK_STATUS_NULL_POINTER if an argument
 *         is NULL, or STACK_STATUS_EMPTY if the stack has no elements.
 */
stackStatus_e stack_pop(stack_t *stack, int *value);

/**
 * @brief Peek at the top value of the stack without removing it.
 *
 * @param stack Pointer to the stack. Must not be NULL.
 * @param value Output parameter that receives the top value. Must not be NULL.
 *
 * @return STACK_STATUS_OK on success, STACK_STATUS_NULL_POINTER if an argument
 *         is NULL, or STACK_STATUS_EMPTY if the stack has no elements.
 */
stackStatus_e stack_peek(const stack_t *stack, int *value);

/**
 * @brief Get the number of elements in the stack.
 *
 * @param stack Pointer to the stack. Must not be NULL.
 * @param size  Output parameter that receives the element count. Must not be NULL.
 *
 * @return STACK_STATUS_OK on success or STACK_STATUS_NULL_POINTER if an argument
 *         is NULL.
 */
stackStatus_e stack_size(const stack_t *stack, size_t *size);

/**
 * @brief Determine whether the stack is empty.
 *
 * @param stack   Pointer to the stack. Must not be NULL.
 * @param isEmpty Output parameter that receives true when the stack is empty
 *                and false otherwise. Must not be NULL.
 *
 * @return STACK_STATUS_OK on success or STACK_STATUS_NULL_POINTER if an argument
 *         is NULL.
 */
stackStatus_e stack_is_empty(const stack_t *stack, bool *isEmpty);

#endif /* STACK_H */
