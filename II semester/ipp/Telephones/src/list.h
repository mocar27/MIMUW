/** @file
 * List class interface.
 *
 * @author Konrad Mocarski
 * @copyright University of Warsaw
 * @date 2022
 */

#ifndef PHONE_NUMBERS_LIST_H
#define PHONE_NUMBERS_LIST_H

/**
 * This is the structure representing the reverse redirect list item.
 */
struct ListElement;

/**
 * This is a structure definition for a reverse redirect list item.
 */
typedef struct ListElement ListElement;

/** @brief The function creates a new list item and copies it
 * @p argString to a new list item.
 * If memory allocation fails, the function returns NULL.
 * @param[in] argString - copied string.
 * @return New element @p ListElement,
 * containing a copy of the string @p argString or NULL,
 * when memory allocation fails.
 */
ListElement *createListElement(char const *argString);

/** @brief The function adds @p argList to the head of the list.
 * Function sets @p first to @p argList when @p first is NULL.
 * @param[in] first - pointer to the head of the list.
 * @param[in,out] argCounter - counter of how many list items there are.
 * @param[in] argList - pointer to a new list item to be added.
 */
void addList(ListElement **first, size_t *argCounter, ListElement *argList);

/** @brief A function that iterates through all the items in the list
 * in a given trie node and listing reverse redirections for a given number
 * (phone number from the root of the tree).
 * The function allocates memory and adds strings to the array @p result.
 * The function stops if memory allocation has failed.
 * @param[in,out] result - an array with reverse redirections @p argString.
 * @param[in,out] argCounter - number of phone numbers in the table @p result.
 * @param[in] argString - the number that we are looking for reverse redirects.
 * @param[in] first - pointer to the head of the list.
 * @param[in] numLength - length of @p argString.
 * @param[in] numIt - iterator by phone number, if it happens to be necessary
 *                    to write down its subsequent digits.
 */
void getReverseList(char **result, size_t *argCounter, char const *argString,
                    ListElement *first, size_t numLength, size_t numIt);

/** @brief The function removes one item from the list of reverse redirects
 * - number that when calling @ref compareString with number
 * @p argStringToDelete returns @p true.
 * @param[in,out] first - pointer to the head of the list.
 * @param[in,out] argCounter - counter of how many list items there are.
 * @param[in] argStringToDelete - number to be removed from the list.
 */
void deleteListElement(ListElement **first, size_t *argCounter,
                       char const *argStringToDelete);

/** @brief Function deletes every element of the list that in number,
 * in given element of the list, contains prefix @p argStringToDelete.
 * @param[in,out]  first - pointer to the head of the list.
 * @param[in,out] argCounter - counter of how many list items there are.
 * @param[in] argStringToDelete - prefix of numbers to be deleted.
 */
void deleteListPrefixes(ListElement **first, size_t *argCounter,
                        char const *argStringToDelete);

/** @brief The function removes the list from memory.
 * The function does nothing, when @p first is NULL.
 * @param[in,out] first - pointer to the head of the list.
 */
void deleteList(ListElement **first);

#endif //PHONE_NUMBERS_LIST_H
