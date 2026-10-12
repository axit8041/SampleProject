#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Status codes returned by the queue API.
 */
typedef enum {
        QUEUE_STATUS_OK = 0,              /**< Operation completed successfully. */
        QUEUE_STATUS_NULL_POINTER = 1,    /**< A required pointer argument was NULL. */
        QUEUE_STATUS_EMPTY = 2,         /**< The queue contains no elements. */
        QUEUE_STATUS_MEMORY_ERROR = 3   /**< Memory allocation failed. */
} queueStatus_e;

/**
 * @brief Internal node of the queue linked list.
 */
typedef struct queueNode {
        int data;                    /**< Value stored in the node. */
        struct queueNode *nextNode;  /**< Pointer to the next node toward the rear. */
} queueNode_t;

/**
 * @brief Queue container implemented as a linked list.
 */
typedef struct {
        queueNode_t *frontNode;       /**< Node at the front of the queue. */
        queueNode_t *rearNode;        /**< Node at the rear of the queue. */
        size_t nodeCount;             /**< Number of elements currently stored. */
} queue_t;

/**
 * @brief Allocate and initialize a new queue.
 *
 * @param queue Output pointer that receives the newly created queue.
 *              Must not be NULL.
 *
 * @return QUEUE_STATUS_OK on success, QUEUE_STATUS_NULL_POINTER if queue is NULL,
 *         or QUEUE_STATUS_MEMORY_ERROR if allocation fails.
 */
queueStatus_e queue_create(queue_t **queue);

/**
 * @brief Destroy a queue and free all associated memory.
 *
 * @param queue Pointer to the queue to destroy. Passing NULL is safe and
 *              returns QUEUE_STATUS_NULL_POINTER.
 *
 * @return QUEUE_STATUS_OK on success or QUEUE_STATUS_NULL_POINTER if queue is NULL.
 */
queueStatus_e queue_destroy(queue_t *queue);

/**
 * @brief Add a value to the rear of the queue.
 *
 * @param queue Pointer to the queue. Must not be NULL.
 * @param value Integer value to enqueue.
 *
 * @return QUEUE_STATUS_OK on success, QUEUE_STATUS_NULL_POINTER if queue is NULL,
 *         or QUEUE_STATUS_MEMORY_ERROR if a new node cannot be allocated.
 */
queueStatus_e queue_enqueue(queue_t *queue, int value);

/**
 * @brief Remove and return the value at the front of the queue.
 *
 * @param queue Pointer to the queue. Must not be NULL.
 * @param value Output parameter that receives the dequeued value. Must not be NULL.
 *
 * @return QUEUE_STATUS_OK on success, QUEUE_STATUS_NULL_POINTER if an argument
 *         is NULL, or QUEUE_STATUS_EMPTY if the queue has no elements.
 */
queueStatus_e queue_dequeue(queue_t *queue, int *value);

/**
 * @brief Peek at the front value of the queue without removing it.
 *
 * @param queue Pointer to the queue. Must not be NULL.
 * @param value Output parameter that receives the front value. Must not be NULL.
 *
 * @return QUEUE_STATUS_OK on success, QUEUE_STATUS_NULL_POINTER if an argument
 *         is NULL, or QUEUE_STATUS_EMPTY if the queue has no elements.
 */
queueStatus_e queue_peek(const queue_t *queue, int *value);

/**
 * @brief Get the number of elements in the queue.
 *
 * @param queue Pointer to the queue. Must not be NULL.
 * @param size  Output parameter that receives the element count. Must not be NULL.
 *
 * @return QUEUE_STATUS_OK on success or QUEUE_STATUS_NULL_POINTER if an argument
 *         is NULL.
 */
queueStatus_e queue_size(const queue_t *queue, size_t *size);

/**
 * @brief Determine whether the queue is empty.
 *
 * @param queue   Pointer to the queue. Must not be NULL.
 * @param isEmpty Output parameter that receives true when the queue is empty
 *                and false otherwise. Must not be NULL.
 *
 * @return QUEUE_STATUS_OK on success or QUEUE_STATUS_NULL_POINTER if an argument
 *         is NULL.
 */
queueStatus_e queue_is_empty(const queue_t *queue, bool *isEmpty);

#endif /* QUEUE_H */
