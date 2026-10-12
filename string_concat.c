#include <stdio.h>
#include <string.h>

/**
 * @brief First hardcoded input string used by the program.
 */
#define FIRST_STRING "Virat"

/**
 * @brief Second hardcoded input string used by the program.
 */
#define SECOND_STRING "Kohli"

/**
 * @brief Size of the buffer that holds the concatenated result.
 *
 * The size is computed from the two hardcoded literals plus space for the
 * terminating null byte.
 */
#define COMBINED_SIZE (((sizeof(FIRST_STRING) - 1) + (sizeof(SECOND_STRING) - 1)) + 1)

/**
 * @brief Concatenates two strings into the provided output buffer.
 *
 * @param outputBuffer     Destination buffer for the concatenated string.
 * @param outputBufferSize Total size of @p outputBuffer in bytes.
 * @param firstString      First source string to copy.
 * @param secondString     Second source string to append.
 *
 * @return 0 on success, non-zero if the destination buffer is too small.
 */
static int string_concat(char *outputBuffer, size_t outputBufferSize,
                         const char *firstString, const char *secondString)
{
        size_t firstLength = strlen(firstString);
        size_t secondLength = strlen(secondString);

        if (outputBufferSize < (firstLength + secondLength + 1)) {
                return 1;
        }

        size_t writeIndex = 0;
        for (size_t index = 0; index < firstLength; index++) {
                outputBuffer[writeIndex] = firstString[index];
                writeIndex++;
        }
        for (size_t index = 0; index < secondLength; index++) {
                outputBuffer[writeIndex] = secondString[index];
                writeIndex++;
        }
        outputBuffer[writeIndex] = '\0';

        return 0;
}

/**
 * @brief Reverses the characters of a string in place.
 *
 * @param stringBuffer Pointer to the string to reverse.
 * @param stringLength Number of characters in the string.
 *
 * @return 0 on success, non-zero if @p stringBuffer is NULL.
 */
static int string_reverse(char *stringBuffer, size_t stringLength)
{
        if (stringBuffer == NULL) {
                return 1;
        }

        size_t leftIndex = 0;
        size_t rightIndex = (stringLength > 0) ? (stringLength - 1) : 0;

        while (leftIndex < rightIndex) {
                char tempChar = stringBuffer[leftIndex];
                stringBuffer[leftIndex] = stringBuffer[rightIndex];
                stringBuffer[rightIndex] = tempChar;
                leftIndex++;
                rightIndex--;
        }

        return 0;
}

/**
 * @brief Entry point for the String Concat program.
 *
 * Concatenates the two hardcoded strings "Virat" and "Kohli", reverses the
 * combined result, and prints it to standard output.
 *
 * @return 0 on success, non-zero on failure.
 */
int main(void)
{
        char combinedString[COMBINED_SIZE];
        int returnValue = string_concat(combinedString, sizeof(combinedString),
                                        FIRST_STRING, SECOND_STRING);
        if (returnValue != 0) {
                return 1;
        }

        size_t combinedLength = strlen(combinedString);
        returnValue = string_reverse(combinedString, combinedLength);
        if (returnValue != 0) {
                return 1;
        }

        int bytesWritten = printf("%s\n", combinedString);
        if (bytesWritten < 0) {
                return 1;
        }

        return 0;
}
