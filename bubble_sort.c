#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Sorts an integer array in ascending order using bubble sort.
 *
 * @param inputArray   Array of integers to sort.
 * @param elementCount Number of elements in the array.
 */
static void bubble_sort(int *inputArray, size_t elementCount)
{
        for (size_t outerIndex = 0; outerIndex < elementCount; outerIndex++) {
                for (size_t innerIndex = 0; innerIndex + 1 < elementCount - outerIndex; innerIndex++) {
                        if (inputArray[innerIndex] > inputArray[innerIndex + 1]) {
                                int tempValue = inputArray[innerIndex];
                                inputArray[innerIndex] = inputArray[innerIndex + 1];
                                inputArray[innerIndex + 1] = tempValue;
                        }
                }
        }
}

/**
 * @brief Entry point for the Bubble Sort program.
 *
 * Reads integers from standard input into an array, sorts the array using
 * bubble sort, and prints the sorted values to standard output.
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
                        int *resizedArray = realloc(inputArray, newCapacity * sizeof(*inputArray));
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

        bubble_sort(inputArray, elementCount);

        for (size_t index = 0; index < elementCount; index++) {
                int bytesWritten = printf("%d", inputArray[index]);
                if (bytesWritten < 0) {
                        free(inputArray);
                        return 1;
                }
                if (index + 1 < elementCount) {
                        bytesWritten = printf(" ");
                        if (bytesWritten < 0) {
                                free(inputArray);
                                return 1;
                        }
                }
        }
        printf("\n");

        free(inputArray);
        return 0;
}
