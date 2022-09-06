/** @file
 * Interface of a class that performs string operations.
 *
 * @author Konrad Mocarski
 * @copyright University of Warsaw
 * @date 2022
 */

#ifndef PHONE_NUMBERS_STRING_H
#define PHONE_NUMBERS_STRING_H

#include <stdbool.h>

/** @brief A function that returns the length of a string.
 * @param[in] argString - given string.
 * @return The length of the given string.
 */
size_t stringLength(char const *argString);

/** @brief A function that compares two strings.
 * @param[in] argString1 - the first string to be compared.
 * @param[in] argString2 - the second string to be compared.
 * @return @p true, if the strings are the same, @p false else.
 */
bool compareString(char const *argString1, char const *argString2);

/** @brief String copy function.
 * The function allocates memory in the passed parameter @p destination,
 * and then copies the string from the parameter @p source.
 * @param[in, out] destination - the variable where the string is copied.
 * @param[in] source - copied string.
 */
void copyString(char **destination, char const *source);

/** @brief A function that validates a string.
 * A function that checks whether the string is non-empty
 * and whether it does not contain characters less than '0'
 * (besides symbol '*' and '#') or greater than '9'.
 * @param[in] argString - string to be checked.
 * @return @p true, if the string is valid, @p false else.
 */
bool checkString(char const *argString);

/** @brief A function that compares two strings
 * and returns a value  to function @ref phoneNumbersCompare,
 * which then returns the right value to the library qsort function.
 * @param[in] argString1 - the first string to be compared.
 * @param[in] argString2 - the second string to be compared.
 * @return Matching to the qsort function comparison value.
 */
int compareStringQsort(char const *argString1, char const *argString2);

/** @brief A function that checks whether @p prefix
 * is the prefix of given phone number @p argString.
 * @param[in] prefix - string prefix.
 * @param[in] argString - string to be checked.
 * @return @p true, if @p prefix is string prefix of @p argString,
 * @p false else.
 */
bool isPrefix(char const *prefix, char const *argString);

#endif //PHONE_NUMBERS_STRING_H