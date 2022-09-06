/** @file
 * Implementation of a class that performs string operations.
 *
 * @author Konrad Mocarski
 * @copyright University of Warsaw
 * @date 2022
 */

#include <stdlib.h>
#include <stdbool.h>
#include "string.h"


size_t stringLength(char const *argString) {

    if (argString == NULL) {
        return 0;
    }

    size_t length = 0;

    while (*(argString + length) != '\0') {
        length++;
    }

    return length;
}


bool compareString(char const *argString1, char const *argString2) {

    if (stringLength(argString1) != stringLength(argString2)) {
        return false;
    }

    size_t it = 0;

    while (*(argString1 + it) != '\0' && *(argString2 + it) != '\0') {

        if (*(argString1 + it) != *(argString2 + it)) {
            return false;
        }

        it++;
    }

    return true;
}


void copyString(char **destination, char const *source) {

    size_t length = stringLength(source);

    *destination = calloc(length + 3, sizeof(char));
    if (*destination == NULL) {
        return;
    }

    for (size_t it = 0; it < length; it++) {
        (*destination)[it] = source[it];
    }
}


bool checkString(char const *argString) {

    if (argString == NULL) {
        return false;
    }

    if (*argString == '\0') { // Empty number is invalid number.
        return false;
    }

    size_t it = 0;

    while (*(argString + it) != '\0') {

        if (*(argString + it) < '0') {

            bool ok = false;
            if (*(argString + it) == '*' || *(argString + it) == '#') {
                ok = true;
            }

            if (!ok) {
                return false;
            }
        }

        if (*(argString + it) > '9') {
            return false;
        }

        it++;
    }

    return true;
}


int compareStringQsort(char const *argString1, char const *argString2) {

    if (argString1 == NULL && argString2 == NULL) {
        return 0;
    }
    if (argString1 == NULL) {
        return 1;
    }
    if (argString2 == NULL) {
        return -1;
    }

    size_t it = 0;

    while (*(argString1 + it) != '\0' && *(argString2 + it) != '\0') {

        if (*(argString1 + it) != *(argString2 + it)) {

            if (*(argString1 + it) < *(argString2 + it)) {
                if (*(argString1 + it) == '*' || *(argString1 + it) == '#') {
                    return 1;
                } else {
                    return -1;
                }
            } else {
                if (*(argString2 + it) == '*' || *(argString2 + it) == '#') {
                    return -1;
                } else {
                    return 1;
                }
            }
        }

        it++;
    }

    if (*(argString1 + it) == '\0' && *(argString2 + it) != '\0') {
        return -1;
    }

    if (*(argString1 + it) != '\0' && *(argString2 + it) == '\0') {
        return 1;
    }

    return 0;
}


bool isPrefix(char const *prefix, char const *argString) {

    if (prefix == NULL || argString == NULL) {
        return false;
    }
    if (stringLength(prefix) > stringLength(argString))
        return false;

    size_t it = 0;

    while (*(prefix + it) != '\0') {

        if (*(prefix + it) != *(argString + it)) {
            return false;
        }

        it++;
    }

    return true;
}
