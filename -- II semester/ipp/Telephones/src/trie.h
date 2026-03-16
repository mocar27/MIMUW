/** @file
 * Trie class interface.
 *
 * @author Konrad Mocarski
 * @copyright University of Warsaw
 * @date 2022
 */

#ifndef PHONE_NUMBERS_TRIE_H
#define PHONE_NUMBERS_TRIE_H

#include <stdbool.h>

/** @brief This is the structure representing the node in the redirect tree.
 *  The structure, in every node of the tree, contains
 *  an array with pointers to successive structures of the same type,
 *  if a given number - counting from the root is redirected,
 *  contained in a given node its redirection. The structure
 *  also contains a pointer to the head of the list of reverse redirections
 *  and the number of reverse redirections of a given number.
 */
struct Trie;

/**
 * This is definition of the Trie structure.
 */
typedef struct Trie Trie;

/** @brief The function creates a new structure of Trie type.
 * The function allocates memory and returns a pointer to the structure.
 * @return Pointer to the new structure - Trie type tree node
 * or NULL if memory allocation failed.
 */
Trie *createTrieNode();

/** @brief The function that adds a telephone number to the tree.
 * Function adds given telephone number to the tree and adds its redirection.
 * If the number is already redirected to a number other than that given
 * to the function, so then the function overwrites the previous redirect,
 * else function does nothing.
 * @param[in,out] argTrie - tree root.
 * @param[in,out] argCounter - counter how many nodes there are in the tree.
 * @param[in] argString - phone number being added.
 * @param[in] argString1 - argString number redirection.
 */
void addForwardTrie(Trie *argTrie, size_t *argCounter,
                    char const *argString, char const *argString1);

/** @brief The function that adds a telephone number to the tree.
 * The function adds a telephone number to the tree
 * and adds a new element to the list - a number,
 * which redirects to the given number.
 * @param[in,out] argTrie - tree root.
 * @param[in,out] argCounter - counter how many nodes there are in the tree.
 * @param[in] argString - phone number being added.
 * @param[in] argString1 - argString number redirection.
 */
void addReverseForwardTrie(Trie *argTrie, size_t *argCounter,
                           char const *argString, char const *argString1);

/** @brief The function allocates memory and fills an array with all of the
 * numbers that are redirected to a given number.
 * @param[in] argTrie - root of the redirect tree.
 * @param[in] argString - the number we are looking for reverse redirects.
 * @param[in, out] argCounter - amount of numbers in the reverse redirection table.
 * @return An array with numbers being the reverse number redirection of @p num.
 */
char **getReverseForwardTrie(Trie *argTrie, char const *argString,
                             size_t *argCounter);

/** @brief Function looking for a given string in the tree.
 * The function searches for a given telephone number in the tree.
 * @param[in] argTrie -  root of the redirect tree.
 * @param[in] argString - the number that function is looking for in the tree.
 * @return @p true, if the number is in the tree, @p false else.
 */
bool findTrie(Trie *argTrie, char const *argString);

/** @brief Function looking for a given string in the tree.
 * The function searches for a given telephone number in the tree,
 * and then if the number is redirected the function returns a pointer
 * to the number to which the given number in the tree is redirected.
 * @param[in] argTrie - tree root.
 * @param[in] argString - the number that function is looking for in the tree.
 * @param[in, out] position - farthest index position number
 *                            in the forwarded number.
 * @return Pointer to the string of the number to which given number
 * is redirected in the tree or NULL if the given number has not been forwarded.
 */
char *findForwardTrie(Trie *argTrie, char const *argString, size_t *position);

/** @brief A function that checks how many children a given node has.
 * @param[in] argTrie - the node that is being checked.
 * @return Number of children of a given node.
 */
size_t hasChildrenTrie(Trie *argTrie);

/** @brief A function that passes through all the digits of a given
 * phone number and returns a pointer to the node where the final digit is.
 * @param[in] argTrie - root of the redirect tree.
 * @param[in] argString - given phone number.
 * @return Pointer to the node where the trailing digit is located
 * or NULL when the passed tree root is NULL.
 */
Trie *skipNodesTrie(Trie *argTrie, char const *argString);

/** @brief The function removes a single reverse redirect.
 * The function removes from the reverse redirections list
 * of the number @p argString this element that contains
 * in its node number @p argStringToDelete
 * (there is only one that number on the whole list).
 * @param[in] argTrieRoot - root of the redirect tree.
 * @param[in] argString  - number for which we want to delete
 *                         @p argStringToDelete from reverse redirections list.
 * @param[in] argStringToDelete - number to delete from the list of reverse
 *                                redirections of number @p argString.
 */
void removeReverseOverrideTrie(Trie *argTrieRoot, char const *argString,
                               char const *argStringToDelete);

/** @brief The function deletes all numbers,
 * which contain prefix @p argStringToDelete
 * and are on the list for the number @p argString.
 * @param[in] argTrieRoot - the root of the entire redirection tree.
 * @param[in] argString - number for which we wane to delete reverse
 *                        redirections for given prefix.
 * @param[in] argStringToDelete - prefix of to be deleted numbers.
 */
void removeReverseTrie(Trie *argTrieRoot, char const *argString,
                       char const *argStringToDelete);

/** @brief A function that removes data from a subtree
 * and redirection of all numbers in this subtree.
 * If any number is redirected to the number we want
 * delete, only the redirection of this node is removed.
 * The function also calls the function @ref removeReverseTrie
 * to remove the reverse redirection of the given number.
 * @param[in,out] argTrie - root of the redirect tree.
 * @param[in] subTreeRoot - root of the redirect tree.
 *                          that we want to free outside the recursion.
 * @param[in] argTrieRoot - the root of the entire redirection tree.
 * @param[in] argString - pointer to the string representing
 *                        prefix of to be deleted numbers.
 */
void removeTrie(Trie **argTrie, Trie *subTreeRoot,
                Trie *argTrieRoot, char const *argString);

/** @brief The function removes one number from the tree.
 * The function removes a given number from memory,
 * until the encountered node has no other children.
 * To prevent deleting the entire redirect tree,
 * we also pass the root of the whole tree again to the function.
 * @param[in,out] argTrie - tree root.
 * @param[in] treeRoot - tree root.
 * @param[in] argString - phone number being deleted.
 * @param[in] it - iterator on the phone number.
 */
void deleteNumberTrie(Trie **argTrie, Trie *treeRoot,
                      char const *argString, size_t it);

/** @brief A function iteratively removes the entire structure.
 * The function deletes the entire structure and its elements from memory,
 * besides the root, that we want to delete outside the function.
 * @param[in,out] argTrie - root of the tree being deleted.
 * @param[in] argCounter - number of nodes in the redirection tree.
 */
void deleteTrieNonRecursive(Trie **argTrie, size_t argCounter);

#endif //PHONE_NUMBERS_TRIE_H