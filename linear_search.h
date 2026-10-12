#ifndef LINEAR_SEARCH_H
#define LINEAR_SEARCH_H

#include <stddef.h>

/**
 * @brief Status codes returned by the linear_search function.
 */
typedef enum {
        LINEAR_SEARCH_STATUS_FOUND = 0,          /**< Target value was found in the array. */
        LINEAR_SEARCH_STATUS_NOT_FOUND = 1,      /**< Target value was not found in the array. */
        LINEAR_SEARCH_STATUS_INVALID_ARGUMENT = 2 /**< A NULL pointer or invalid argument was supplied. */
} linearSearchStatus_e;

/**
 * @brief Search for a target value in an integer array using linear search.
 *
 * The array is inspected from the first element toward the last element. If the
 * target appears more than once, the index of the first occurrence is returned.
 * The array does not need to be sorted.
 *
 * @param inputArray   Array of integers to search. Must not be NULL when
 *                     elementCount is greater than zero.
 * @param elementCount Number of elements in the array.
 * @param targetValue  Value to search for.
 * @param foundIndex   Output parameter that receives the index of the first
 *                     occurrence when the target is found. Ignored when the
 *                     target is not found.
 *
 * @return LINEAR_SEARCH_STATUS_FOUND when the target is found,
 *         LINEAR_SEARCH_STATUS_NOT_FOUND when the target is absent,
 *         or LINEAR_SEARCH_STATUS_INVALID_ARGUMENT for invalid input.
 */
linearSearchStatus_e linear_search(const int *inputArray, size_t elementCount,
                                   int targetValue, size_t *foundIndex);

#endif /* LINEAR_SEARCH_H */
