#include <stdio.h>

/**
 * @brief Entry point for the Hello World program.
 *
 * Prints a greeting message to standard output and exits successfully.
 *
 * @return 0 on success, non-zero on failure.
 */
int main(void)
{
        int bytesWritten = printf("Hello World\n");

        if (bytesWritten < 0) {
                return 1;
        }

        return 0;
}
