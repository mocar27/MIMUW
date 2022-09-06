/** @file
 * Implementation of the List class.
 *
 * @author Konrad Mocarski
 * @copyright University of Warsaw
 * @date 2022
 */

#include <stdlib.h>
#include "list.h"
#include "string.h"


/**
 * This is the structure representing the reverse redirect list item.
 */
struct ListElement {
    char *reverseForward;   ///< Reverse list owner redirection.
    ListElement *next;      ///< Pointer to the next element of the list.
};


ListElement *createListElement(char const *argString) {

    ListElement *temp = calloc(1, sizeof(ListElement));
    if (temp == NULL) {
        return NULL;
    }

    copyString(&(temp->reverseForward), argString);
    temp->next = NULL;

    return temp;
}


void addList(ListElement **first, size_t *argCounter, ListElement *argList) {

    if (*first == NULL) {
        *first = argList;
        (*argCounter)++;
        return;
    }

    argList->next = *first;
    *first = argList;
    (*argCounter)++;
}


void getReverseList(char **result, size_t *argCounter, char const *argString,
                    ListElement *first, size_t numLength, size_t numIt) {

    if (first == NULL) {
        return;
    }

    ListElement *temp = first;

    while (temp != NULL) {
        char *reverse = calloc(stringLength(temp->reverseForward)
                               + numLength - numIt + 3, sizeof(char));
        if (reverse == NULL) {
            return;
        }

        size_t it = 0;
        size_t numItTemp = numIt;
        while (temp->reverseForward[it] != '\0') {
            reverse[it] = temp->reverseForward[it];
            it++;
        }
        while (argString[numItTemp] != '\0') {
            reverse[it] = argString[numItTemp];
            it++;
            numItTemp++;
        }

        copyString(&result[*argCounter], reverse);
        if (result[*argCounter] == NULL) {
            free(reverse);
            return;
        }

        (*argCounter)++;
        free(reverse);
        temp = temp->next;
    }
}


void deleteListElement(ListElement **first, size_t *argCounter,
                       char const *argStringToDelete) {

    ListElement *previous;

    if (*first != NULL && compareString((*first)->reverseForward,
                                        argStringToDelete)) {
        previous = *first;
        *first = (*first)->next;
        free(previous->reverseForward);
        previous->reverseForward = NULL;
        free(previous);
        previous = NULL;
        (*argCounter)--;
        return;
    }

    if (*first == NULL) {
        return;
    }

    ListElement *temp = (*first)->next;
    previous = *first;

    while (temp != NULL && temp->next != NULL) {

        if (compareString(temp->reverseForward, argStringToDelete)) {
            previous->next = temp->next;
            free(temp->reverseForward);
            temp->reverseForward = NULL;
            free(temp);
            temp = NULL;
            (*argCounter)--;
            return;
        } else {
            previous = temp;
        }
        temp = temp->next;
    }

    if (temp == NULL) {
        return;
    }

    if (compareString(temp->reverseForward, argStringToDelete)) {

        free(temp->reverseForward);
        temp->reverseForward = NULL;
        free(temp);
        temp = NULL;
        if (previous != NULL) {
            previous->next = NULL;
        }
        (*argCounter)--;
    }
}


void deleteListPrefixes(ListElement **first, size_t *argCounter,
                        char const *argStringToDelete) {

    ListElement *previous;

    while (*first != NULL && isPrefix(argStringToDelete,
                                      (*first)->reverseForward)) {
        previous = *first;
        *first = (*first)->next;
        free(previous->reverseForward);
        previous->reverseForward = NULL;
        free(previous);
        previous = NULL;
        (*argCounter)--;
    }

    if (*first == NULL) {
        return;
    }

    ListElement *temp = (*first)->next;
    previous = *first;

    while (temp != NULL && temp->next != NULL) {

        if (isPrefix(argStringToDelete, temp->reverseForward)) {
            previous->next = temp->next;
            free(temp->reverseForward);
            temp->reverseForward = NULL;
            free(temp);
            temp = NULL;
            (*argCounter)--;
            temp = previous;
        } else {
            previous = temp;
        }
        temp = temp->next;
    }

    if (temp != NULL && isPrefix(argStringToDelete, temp->reverseForward)) {

        free(temp->reverseForward);
        temp->reverseForward = NULL;
        free(temp);
        temp = NULL;
        if (previous != NULL) {
            previous->next = NULL;
        }
        (*argCounter)--;
    }
}


void deleteList(ListElement **first) {

    if (*first == NULL) {
        return;
    }

    ListElement *temp = *first;

    while (temp != NULL) {
        temp = temp->next;
        free((*first)->reverseForward);
        (*first)->reverseForward = NULL;
        free(*first);
        *first = temp;

    }
    if (*first != NULL) {
        if ((*first)->reverseForward != NULL) {
            free((*first)->reverseForward);
            (*first)->reverseForward = NULL;
        }
        free(*first);
        *first = NULL;
    }
}