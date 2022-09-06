/** @file
 * Implementation of the Trie class.
 *
 * @author Konrad Mocarski
 * @copyright University of Warsaw
 * @date 2022
 */

#include <stdlib.h>
#include <stdbool.h>
#include "trie.h"
#include "string.h"
#include "list.h"


/**
 * This is a constant that represents the number of digits 0 through 9,
 * and the characters of the numbers 10 and 11.
 */
#define DIGITS 12


/**
 * This is the structure representing the node in the redirect tree.
 */
struct Trie {
    Trie *children[DIGITS]; ///< Array of pointers to successive nodes in the tree.
    char *forward;          ///< Number to which the given number is redirected.
    size_t reverseCounter;  ///< Amount of numbers redirecting to a given number.
    ListElement *first;     ///< The head of the list of reverse redirects.
};


Trie *createTrieNode() {

    Trie *temp = calloc(1, sizeof(Trie));
    if (temp == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < DIGITS; i++) {
        temp->children[i] = NULL;
    }

    temp->forward = NULL;
    temp->first = NULL;
    temp->reverseCounter = 0;


    return temp;
}


void addForwardTrie(Trie *argTrie, size_t *argCounter,
                    char const *argString, char const *argString1) {

    if (argTrie == NULL) {
        return;
    }

    size_t index;
    Trie *temp = argTrie;

    for (size_t i = 0; *(argString + i) != '\0'; i++) {

        switch (*(argString + i)) {
            case '*':
                index = 10;
                break;
            case '#':
                index = 11;
                break;
            default:
                index = *(argString + i) - '0';
                break;
        }

        if (temp->children[index] == NULL) {

            /* If any digit from argString is not in the tree,
             * then we create a new node. */
            temp->children[index] = createTrieNode();
            (*argCounter)++;
            if (temp->children[index] == NULL) {
                deleteTrieNonRecursive(&argTrie, *argCounter);
                free(argTrie);
                argTrie = NULL;
                return;
            }
        }

        temp = temp->children[index];
    }

    // Overwriting the redirection.
    if (temp != NULL) {
        if (temp->forward != NULL) {

            /* If the specified redirection is identical,
             * like the current redirection, the function does nothing. */
            if (compareString(temp->forward, argString1)) {
                return;
            }

            removeReverseOverrideTrie(argTrie, temp->forward, argString);
            free(temp->forward);
            temp->forward = NULL;
        }

        copyString(&(temp->forward), argString1);
        addReverseForwardTrie(argTrie, argCounter, argString1, argString);
    }
}


void addReverseForwardTrie(Trie *argTrie, size_t *argCounter,
                           char const *argString, char const *argString1) {

    if (argTrie == NULL) {
        return;
    }

    size_t index;
    Trie *temp = argTrie;

    for (size_t i = 0; *(argString + i) != '\0'; i++) {

        switch (*(argString + i)) {
            case '*':
                index = 10;
                break;
            case '#':
                index = 11;
                break;
            default:
                index = *(argString + i) - '0';
                break;
        }

        if (temp->children[index] == NULL) {

            /* If any digit from argString is not in the tree,
             * then we create a new node. */
            temp->children[index] = createTrieNode();
            (*argCounter)++;
            if (temp->children[index] == NULL) {
                deleteTrieNonRecursive(&argTrie, *argCounter);
                free(argTrie);
                argTrie = NULL;
                return;
            }
        }

        temp = temp->children[index];
    }

    ListElement *new = createListElement(argString1);
    addList(&temp->first, &temp->reverseCounter, new);
}


char **getReverseForwardTrie(Trie *argTrie, char const *argString,
                             size_t *argCounter) {

    Trie *temp = argTrie;
    size_t index;
    size_t length = stringLength(argString);
    size_t extend = 100;

    char **result = calloc(extend, sizeof(char *));
    if (result == NULL) {
        return NULL;
    }

    copyString(&result[*argCounter], argString);
    (*argCounter)++;

    for (size_t i = 0; *(argString + i) != '\0'; i++) {

        switch (*(argString + i)) {
            case '*':
                index = 10;
                break;
            case '#':
                index = 11;
                break;
            default:
                index = *(argString + i) - '0';
                break;
        }

        if (temp->reverseCounter > 0) { // Saving reverse redirects.
            if (*argCounter == extend) {
                extend *= 2;
                result = realloc(result, extend * sizeof(char *));
                if (result == NULL) {
                    return NULL;
                }
            }
            getReverseList(result, argCounter, argString,
                           temp->first, length, i);
        }

        if (temp->children[index] == NULL) {
            return result;
        }

        temp = temp->children[index];
    }

    if (temp->reverseCounter > 0) { // Saving reverse redirects.
        if (*argCounter == extend) {
            extend *= 2;
            result = realloc(result, extend * sizeof(char *));
            if (result == NULL) {
                return NULL;
            }
        }
        getReverseList(result, argCounter, argString,
                       temp->first, length, length);
    }

    return result;
}


bool findTrie(Trie *argTrie, char const *argString) {

    if (argTrie == NULL || !checkString(argString)) {
        return false;
    }

    size_t index;
    Trie *temp = argTrie;

    for (size_t i = 0; *(argString + i) != '\0'; i++) {

        switch (*(argString + i)) {
            case '*':
                index = 10;
                break;
            case '#':
                index = 11;
                break;
            default:
                index = *(argString + i) - '0';
                break;
        }

        if (temp->children[index] == NULL) {
            return false;
        }

        temp = temp->children[index];
    }

    return true;
}


char *findForwardTrie(Trie *argTrie, char const *argString, size_t *position) {

    if (argTrie == NULL || !checkString(argString)) {
        return NULL;
    }

    size_t length = stringLength(argString);
    size_t index;
    char *result = NULL;
    Trie *temp = argTrie;

    for (size_t i = 0; *(argString + i) != '\0'; i++) {

        switch (*(argString + i)) {
            case '*':
                index = 10;
                break;
            case '#':
                index = 11;
                break;
            default:
                index = *(argString + i) - '0';
                break;
        }

        if (temp->forward != NULL) {
            result = temp->forward;
            (*position) = i - 1;
        }

        if (temp->children[index] == NULL) {
            return result;
        }

        temp = temp->children[index];
    }

    if (temp != NULL && temp->forward != NULL) {
        result = temp->forward;
        (*position) = length - 1;
    }

    return result;
}


size_t hasChildrenTrie(Trie *argTrie) {

    size_t children = 0;

    for (size_t i = 0; i < DIGITS; i++) {

        if (argTrie->children[i] != NULL) {
            children++;
        }
    }

    return children;
}


Trie *skipNodesTrie(Trie *argTrie, char const *argString) {

    if (argTrie == NULL) {
        return NULL;
    }

    Trie *temp = argTrie;
    size_t index;

    for (size_t i = 0; *(argString + i) != '\0'; i++) {

        switch (*(argString + i)) {
            case '*':
                index = 10;
                break;
            case '#':
                index = 11;
                break;
            default:
                index = *(argString + i) - '0';
                break;
        }

        temp = temp->children[index];
    }

    return temp;
}


void removeReverseOverrideTrie(Trie *argTrieRoot, char const *argString,
                               char const *argStringToDelete) {
    Trie *temp = argTrieRoot;
    size_t index;

    for (size_t i = 0; *(argString + i) != '\0'; i++) {

        switch (*(argString + i)) {
            case '*':
                index = 10;
                break;
            case '#':
                index = 11;
                break;
            default:
                index = *(argString + i) - '0';
                break;
        }

        if (temp->children[index] == NULL) {
            return;
        }

        temp = temp->children[index];
    }

    deleteListElement(&temp->first, &temp->reverseCounter, argStringToDelete);
}


void removeReverseTrie(Trie *argTrieRoot, char const *argString,
                       char const *argStringToDelete) {

    Trie *temp = argTrieRoot;
    size_t index;

    for (size_t i = 0; *(argString + i) != '\0'; i++) {

        switch (*(argString + i)) {
            case '*':
                index = 10;
                break;
            case '#':
                index = 11;
                break;
            default:
                index = *(argString + i) - '0';
                break;
        }

        if (temp->children[index] == NULL) {
            return;
        }

        temp = temp->children[index];
    }

    deleteListPrefixes(&temp->first, &temp->reverseCounter, argStringToDelete);
}


void removeTrie(Trie **argTrie, Trie *subTreeRoot,
                Trie *argTrieRoot, char const *argString) {

    if (*argTrie == NULL) {
        return;
    }

    for (size_t i = 0; i < DIGITS; i++) {
        if ((*argTrie)->children[i] != NULL) {
            removeTrie(&(*argTrie)->children[i], subTreeRoot,
                       argTrieRoot, argString);
        }
    }

    if ((*argTrie)->forward != NULL) {
        removeReverseTrie(argTrieRoot, (*argTrie)->forward, argString);
        free((*argTrie)->forward);
        (*argTrie)->forward = NULL;
    }

    if ((*argTrie)->reverseCounter == 0) {
        if (*argTrie != subTreeRoot && !hasChildrenTrie(*argTrie)) {
            free(*argTrie);
            *argTrie = NULL;
        }
    }
}


void deleteNumberTrie(Trie **argTrie, Trie *treeRoot,
                      char const *argString, size_t it) {

    if (*argTrie == NULL) {
        return;
    }

    if (argString[it] == '\0') {
        free((*argTrie)->forward);
        (*argTrie)->forward = NULL;
        if ((*argTrie)->reverseCounter == 0) {
            free(*argTrie);
            *argTrie = NULL;
        }
        return;
    }

    size_t index;

    switch (argString[it]) {
        case '*':
            index = 10;
            break;
        case '#':
            index = 11;
            break;
        default:
            index = argString[it] - '0';
            break;
    }

    deleteNumberTrie(&(*argTrie)->children[index], treeRoot, argString, it + 1);

    if ((*argTrie)->reverseCounter > 0 || hasChildrenTrie(*argTrie)) {
        return;
    } else if (*argTrie != treeRoot && (*argTrie)->forward == NULL) {
        free(*argTrie);
        *argTrie = NULL;
    }
}


void deleteTrieNonRecursive(Trie **argTrie, size_t argCounter) {

    if (*argTrie == NULL) {
        return;
    }

    Trie **arrayToDelete = calloc(argCounter, sizeof(Trie *));
    if (arrayToDelete == NULL) {
        return;
    }

    arrayToDelete[0] = *argTrie;
    Trie *temp = NULL;
    size_t iterator = 1;

    for (size_t i = 0; i < argCounter; i++) {

        if (arrayToDelete[i] != NULL) {
            temp = arrayToDelete[i];
            for (size_t j = 0; j < DIGITS; j++) {
                if (temp->children[j] != NULL) {
                    arrayToDelete[iterator] = temp->children[j];
                    iterator++;
                }
            }
        }
    }
    for (size_t i = iterator - 1; i > 0; i--) {
        if (arrayToDelete[i] != NULL) {
            free(arrayToDelete[i]->forward);
            arrayToDelete[i]->forward = NULL;
            deleteList(&arrayToDelete[i]->first);
            free(arrayToDelete[i]);
            arrayToDelete[i] = NULL;
        }
    }

    free(arrayToDelete);
}