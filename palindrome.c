#include <ctype.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Size of the buffer that holds the input read from stdin.
 */
#define INPUT_BUFFER_SIZE 1024

/**
 * @brief Checks whether a string is a palindrome.
 *
 * Only alphanumeric characters are considered and the comparison is
 * case-insensitive, so "Madam" and "A man a plan a canal Panama" are both
 * palindromes.
 *
 * @param inputString Pointer to the null-terminated string to check.
 *
 * @return 1 if the filtered input is a palindrome, 0 otherwise.
 */
static int is_palindrome(const char *inputString)
{
        size_t leftIndex = 0;
        size_t rightIndex = strlen(inputString);

        if (rightIndex == 0) {
                return 0;
        }
        rightIndex--;

        while (leftIndex < rightIndex) {
                while (leftIndex < rightIndex &&
                       !isalnum((unsigned char)inputString[leftIndex])) {
                        leftIndex++;
                }
                while (leftIndex < rightIndex &&
                       !isalnum((unsigned char)inputString[rightIndex])) {
                        rightIndex--;
                }
                if (tolower((unsigned char)inputString[leftIndex]) !=
                    tolower((unsigned char)inputString[rightIndex])) {
                        return 0;
                }
                leftIndex++;
                rightIndex--;
        }

        /* Reject inputs that contain no alphanumeric characters at all. */
        if (!isalnum((unsigned char)inputString[leftIndex]) &&
            leftIndex == rightIndex) {
                return 0;
        }

        return 1;
}

/**
 * @brief Entry point for the Palindrome program.
 *
 * Reads one line from standard input, trims leading and trailing whitespace,
 * and prints "Palindrome" when the input reads the same forwards and
 * backwards, otherwise "Not Palindrome".
 *
 * @return 0 when a non-empty input was read, non-zero on failure.
 */
int main(void)
{
        char inputString[INPUT_BUFFER_SIZE];

        if (fgets(inputString, sizeof(inputString), stdin) == NULL) {
                printf("Not Palindrome\n");
                return 1;
        }

        size_t inputLength = strlen(inputString);
        while (inputLength > 0 &&
               isspace((unsigned char)inputString[inputLength - 1])) {
                inputString[inputLength - 1] = '\0';
                inputLength--;
        }

        char *startPointer = inputString;
        while (*startPointer != '\0' &&
               isspace((unsigned char)*startPointer)) {
                startPointer++;
        }

        if (*startPointer == '\0') {
                printf("Not Palindrome\n");
                return 1;
        }

        if (is_palindrome(startPointer)) {
                printf("Palindrome\n");
        } else {
                printf("Not Palindrome\n");
        }

        return 0;
}
