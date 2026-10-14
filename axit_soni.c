#include <stdio.h>

/**
 * @brief Entry point for the Axit Soni program.
 *
 * Prints the name "Axit Soni" to standard output and exits successfully.
 *
 * @return 0 on success, non-zero on failure.
 */
int main(void)
{
        int bytesWritten = printf("Axit Soni\n");

        if (bytesWritten < 0) {
                return 1;
        }

        return 0;
}
