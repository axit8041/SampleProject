#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#define MAX_INPUT 20

/**
 * @brief Computes the factorial of a non-negative integer recursively.
 *
 * @param n Non-negative integer whose factorial is computed.
 * @return n! as an unsigned long long. Valid for n <= 20.
 */
static unsigned long long factorial(unsigned int n)
{
        if (n <= 1) {
                return 1;
        }
        return n * factorial(n - 1);
}

/**
 * @brief Entry point for the factorial program.
 *
 * Reads a single non-negative integer (0-20) from standard input and
 * prints its factorial. Rejects missing, negative, out-of-range,
 * non-numeric, or trailing input with a diagnostic on stderr and a
 * non-zero exit status.
 *
 * @return 0 on success, non-zero on failure.
 */
int main(void)
{
        char buffer[64];
        char *end;
        long value;

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                fprintf(stderr, "factorial: missing input\n");
                return 1;
        }

        errno = 0;
        value = strtol(buffer, &end, 10);
        if (end == buffer || errno != 0) {
                fprintf(stderr, "factorial: invalid input, expected an integer\n");
                return 1;
        }

        while (*end == ' ' || *end == '\t') {
                end++;
        }
        if (*end != '\n' && *end != '\0') {
                fprintf(stderr, "factorial: invalid trailing input\n");
                return 1;
        }

        if (value < 0) {
                fprintf(stderr, "factorial: input must be non-negative\n");
                return 1;
        }
        if (value > MAX_INPUT) {
                fprintf(stderr, "factorial: input must be between 0 and %d\n", MAX_INPUT);
                return 1;
        }

        printf("%llu\n", factorial((unsigned int)value));
        return 0;
}
