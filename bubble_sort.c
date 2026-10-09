#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Sorts an integer array in ascending order using bubble sort.
 *
 * @param arr Array of integers to sort.
 * @param n   Number of elements in the array.
 */
static void bubble_sort(int *arr, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j + 1 < n - i; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
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
    int *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    int value;

    while (scanf("%d", &value) == 1) {
        if (count >= capacity) {
            size_t new_capacity = capacity == 0 ? 8 : capacity * 2;
            int *new_arr = realloc(arr, new_capacity * sizeof(*arr));
            if (new_arr == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                free(arr);
                return 1;
            }
            arr = new_arr;
            capacity = new_capacity;
        }
        arr[count++] = value;
    }

    bubble_sort(arr, count);

    for (size_t i = 0; i < count; i++) {
        printf("%d", arr[i]);
        if (i + 1 < count) {
            printf(" ");
        }
    }
    printf("\n");

    free(arr);
    return 0;
}
