#include <stdio.h>
#include <stdlib.h>

#include "linear_search.h"

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
                                   int targetValue, size_t *foundIndex)
{
        if ((inputArray == NULL) && (elementCount > 0)) {
                return LINEAR_SEARCH_STATUS_INVALID_ARGUMENT;
        }

        if (foundIndex == NULL) {
                return LINEAR_SEARCH_STATUS_INVALID_ARGUMENT;
        }

        for (size_t index = 0; index < elementCount; index++) {
                if (inputArray[index] == targetValue) {
                        *foundIndex = index;
                        return LINEAR_SEARCH_STATUS_FOUND;
                }
        }

        return LINEAR_SEARCH_STATUS_NOT_FOUND;
}

/**
 * @brief Entry point for the Linear Search program.
 *
 * Reads integers from standard input until EOF. The final value is treated as
 * the target to search for; all preceding values form the array. The program
 * prints either the index where the target was first found or a "Not Found"
 * message. The input array may be sorted or unsorted.
 *
 * Input format: space or newline separated integers, terminated by EOF.
 *
 * @return 0 on success, non-zero on failure.
 */
int main(void)
{
        int *inputArray = NULL;
        size_t elementCount = 0;
        size_t currentCapacity = 0;
        int currentValue;

        while (scanf("%d", &currentValue) == 1) {
                if (elementCount >= currentCapacity) {
                        size_t newCapacity = currentCapacity == 0 ? 8 : currentCapacity * 2;
                        int *resizedArray = realloc(inputArray,
                                                      newCapacity * sizeof(*inputArray));
                        if (resizedArray == NULL) {
                                fprintf(stderr, "Memory allocation failed\n");
                                free(inputArray);
                                return 1;
                        }
                        inputArray = resizedArray;
                        currentCapacity = newCapacity;
                }
                inputArray[elementCount++] = currentValue;
        }

        if (elementCount == 0) {
                int bytesWritten = printf("Not Found\n");
                free(inputArray);
                return (bytesWritten < 0) ? 1 : 0;
        }

        size_t arraySize = elementCount - 1;
        int targetValue = inputArray[arraySize];
        size_t foundIndex = 0;

        linearSearchStatus_e searchStatus = linear_search(inputArray, arraySize,
                                                          targetValue, &foundIndex);

        int bytesWritten = 0;
        if (searchStatus == LINEAR_SEARCH_STATUS_FOUND) {
                bytesWritten = printf("Found at index %zu\n", foundIndex);
        } else {
                bytesWritten = printf("Not Found\n");
        }

        free(inputArray);
        return (bytesWritten < 0) ? 1 : 0;
}
