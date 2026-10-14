/**
 * @file factorial.c
 * @brief Standalone command-line factorial calculator.
 *
 * Reads one non-negative integer from standard input, computes its factorial
 * using a recursive function, and prints the result. Only inputs in the
 * range 0-20 are accepted; other inputs are rejected with a diagnostic.
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_FACTORIAL_INPUT 20u

/**
 * @brief Computes the factorial of a non-negative integer recursively.
 *
 * @param n Non-negative integer whose factorial is to be computed.
 * @return The value of n!.
 */
unsigned long long factorial(unsigned int n)
{
    if (n == 0) {
        return 1;
    }

    return n * factorial(n - 1);
}

/**
 * @brief Reads a single non-negative integer from standard input.
 *
 * Validates that the input consists of exactly one integer with no trailing
 * non-whitespace characters.
 *
 * @param[out] value Pointer to store the parsed value.
 * @return 0 on success, non-zero on failure.
 */
int read_input(unsigned long long *value)
{
    char buffer[128];

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        fprintf(stderr, "Error: missing input\n");
        return 1;
    }

    char extra;
    int matched = sscanf(buffer, "%llu %c", value, &extra);

    if (matched != 1) {
        fprintf(stderr, "Error: invalid input (expected a non-negative integer)\n");
        return 1;
    }

    if (*value > MAX_FACTORIAL_INPUT) {
        fprintf(stderr, "Error: input out of range (must be between 0 and %u)\n",
                MAX_FACTORIAL_INPUT);
        return 1;
    }

    return 0;
}

/**
 * @brief Entry point for the factorial program.
 *
 * @return EXIT_SUCCESS on successful computation, EXIT_FAILURE otherwise.
 */
int main(void)
{
    unsigned long long input;

    if (read_input(&input) != 0) {
        return EXIT_FAILURE;
    }

    printf("%llu\n", factorial((unsigned int)input));

    return EXIT_SUCCESS;
}
