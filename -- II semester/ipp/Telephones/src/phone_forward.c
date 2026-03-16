/** @file
 * Implementation of a class that stores redirections of telephone numbers.
 *
 * @author Konrad Mocarski
 * @copyright University of Warsaw
 * @date 2022
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "phone_forward.h"
#include "trie.h"
#include "string.h"


/**
 * This is the structure that stores redirections of telephone numbers.
 */
struct PhoneForward {
    Trie *root;             ///< Root of the redirect tree.
    size_t nodesCounter;    ///< Variable to count number of nodes in the tree.
};


/**
 * This is a structure that stores a sequence of telephone numbers.
 */
struct PhoneNumbers {
    char **numbers;     ///< Phone number array.
    size_t counter;     ///< Variable to count phone numbers in the structure.
};


/** @brief A function that lexicographically compares two telephone
 * numbers and returns matching value to the library qsort function.
 * @param[in] s1 - the first string to be compared.
 * @param[in] s2 - the second string to be compared.
 * @return Matching to the qsort function comparison value.
 */
static int phoneNumbersCompare(void const *s1, void const *s2) {

    if (s1 == NULL && s2 == NULL) {
        return 0;
    }
    if (s1 == NULL) {
        return 1;
    }
    if (s2 == NULL) {
        return -1;
    }

    char const **argString1 = (char const **) s1;
    char const **argString2 = (char const **) s2;

    return compareStringQsort(*argString1, *argString2);
}


/** @brief Creates a new structure.
 * Creates a new structure that contains no phone numbers.
 * @return Pointer to the structure created, or NULL when failed
 *         allocate memory.
 */
static PhoneNumbers *phnumNew(void) {

    PhoneNumbers *temp = calloc(1, sizeof(PhoneNumbers));
    if (temp == NULL) {
        return NULL;
    }

    temp->numbers = calloc(1, sizeof(char *));
    if (temp->numbers == NULL) {
        free(temp);
        return NULL;
    }

    temp->counter = 0;

    return temp;
}


PhoneForward *phfwdNew(void) {

    PhoneForward *temp = calloc(1, sizeof(PhoneForward));
    if (temp == NULL) {
        return NULL;
    }

    temp->nodesCounter = 1;

    temp->root = createTrieNode();
    if (temp->root == NULL) {
        free(temp);
        return NULL;
    }

    return temp;
}


void phfwdDelete(PhoneForward *pf) {

    if (pf == NULL) {
        return;
    }

    deleteTrieNonRecursive(&pf->root, pf->nodesCounter);

    free(pf->root);
    pf->root = NULL;

    free(pf);
    pf = NULL;
}


bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {

    if (pf == NULL) {
        return false;
    }

    if (!checkString(num1) || !checkString(num2)) {
        return false;
    }

    if (compareString(num1, num2)) {
        return false;
    }

    size_t it = 0;

    addForwardTrie(pf->root, &pf->nodesCounter, num1, num2);

    if (findForwardTrie(pf->root, num1, &it) != NULL) {
        return true;
    } else {
        return false;
    }
}


void phfwdRemove(PhoneForward *pf, char const *num) {

    if (pf == NULL || !checkString(num)) {
        return;
    }

    if (!findTrie(pf->root, num)) {
        return;
    }

    Trie *temp = skipNodesTrie(pf->root, num);
    removeTrie(&temp, temp, pf->root, num);

    /* As each number is also its own prefix,
     * then we want to remove "* num" from the tree as well,
     * until there are no other children,
     * does not have its own redirection, and there are no numbers,
     * which redirect to him. */
    if (!hasChildrenTrie(temp)) {
        deleteNumberTrie(&pf->root, pf->root, num, 0);
    }
}


PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {

    if (pf == NULL) {
        return NULL;
    }

    PhoneNumbers *result = phnumNew();
    if (result == NULL) {
        return NULL;
    }

    if (!checkString(num)) {
        return result;
    }

    bool isForward = false;
    size_t pos = 0; // Position of the farthest index in the forwarded number.
    char *forward = findForwardTrie(pf->root, num, &pos);

    if (forward != NULL) {
        isForward = true;
    }

    if (isForward && pos != stringLength(num) - 1) {

        pos++;
        size_t lengthOfForward = stringLength(num) + stringLength(forward)
                                 - pos;
        char *finalForward = calloc(lengthOfForward + 3, sizeof(char));
        size_t i = 0;

        /* At this point, the longest redirect is copied
         * on which the variable "forward" shows, and then all the rest
         * number, which is in the variable "num" after the string that is
         * contained in the array after the 'item' indicated by "pos". */

        while (forward[i] != '\0') {
            finalForward[i] = forward[i];
            i++;
        }

        while (num[pos] != '\0') {
            finalForward[i] = num[pos];
            i++;
            pos++;
        }

        // Finally, we save the copied string in the result array.
        copyString(&(result->numbers[result->counter]), finalForward);
        free(finalForward);

    } else if (isForward) {
        copyString(&(result->numbers[result->counter]), forward);
    }

    // If we go in here, it means the number has not been redirected.
    if (!isForward) {
        copyString(&(result->numbers[result->counter]), num);
    }

    result->counter++;
    return result;
}


PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num) {

    if (pf == NULL) {
        return NULL;
    }

    PhoneNumbers *result = phnumNew();
    if (result == NULL) {
        return NULL;
    }

    if (!checkString(num)) {
        return result;
    }

    free(result->numbers);
    result->numbers = NULL;

    result->numbers = getReverseForwardTrie(pf->root, num, &result->counter);

    qsort(result->numbers, result->counter,
          sizeof(char *), phoneNumbersCompare);

    for (size_t i = 1; i < result->counter; i++) {
        if (compareString(result->numbers[i - 1], result->numbers[i])) {
            memmove(result->numbers[i - 1], result->numbers[result->counter - 1],
                    stringLength(result->numbers[result->counter - 1]) + 1);
            free(result->numbers[result->counter - 1]);
            result->numbers[result->counter - 1] = NULL;
            result->counter--;
        }
    }

    result->numbers = realloc(result->numbers,
                              result->counter * sizeof(char *));
    if (result->numbers == NULL) {
        return NULL;
    }

    qsort(result->numbers, result->counter,
          sizeof(char *), phoneNumbersCompare);

    return result;
}


PhoneNumbers *phfwdGetReverse(PhoneForward const *pf, char const *num) {

    if (pf == NULL) {
        return NULL;
    }

    PhoneNumbers *result = phnumNew();
    if (result == NULL) {
        return NULL;
    }

    if (!checkString(num)) {
        return result;
    }

    phnumDelete(result);
    result = phfwdReverse(pf, num);
    PhoneNumbers *temp;

    for (size_t i = 0; i < result->counter; i++) {

        temp = phfwdGet(pf, result->numbers[i]);
        if (!compareString(temp->numbers[0], num)) {
            memmove(result->numbers[i], result->numbers[result->counter - 1],
                    stringLength(result->numbers[result->counter - 1]) + 1);
            free(result->numbers[result->counter - 1]);
            result->numbers[result->counter - 1] = NULL;
            result->counter--;
            i--;
        }
        phnumDelete(temp);
    }

    if (result->counter != 0) {
        result->numbers = realloc(result->numbers,
                                  result->counter * sizeof(char *));
        if (result->numbers == NULL) {
            return NULL;
        }

        qsort(result->numbers, result->counter,
              sizeof(char *), phoneNumbersCompare);
    }

    return result;
}


void phnumDelete(PhoneNumbers *pnum) {

    if (pnum == NULL) {
        return;
    }

    for (size_t i = 0; i < pnum->counter; i++) {
        free(pnum->numbers[i]);
        pnum->numbers[i] = NULL;
    }

    free(pnum->numbers);
    pnum->numbers = NULL;
    free(pnum);
    pnum = NULL;
}


char const *phnumGet(PhoneNumbers const *pnum, size_t idx) {

    if (pnum == NULL || pnum->counter <= idx) {
        return NULL;
    }

    return pnum->numbers[idx];
}