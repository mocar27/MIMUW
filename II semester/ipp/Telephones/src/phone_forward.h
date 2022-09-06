/** @file
 * Interface of a class that stores redirections of telephone numbers.
 *
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @copyright University of Warsaw
 * @date 2022
 *
 * Modified by Konrad Mocarski
 */

#ifndef __PHONE_FORWARD_H__
#define __PHONE_FORWARD_H__

#include <stdbool.h>
#include <stddef.h>

/**
 * This is the structure that stores redirections of telephone numbers.
 */
struct PhoneForward;

/**
 * This is definition of structure storing redirections of telephone numbers.
 */
typedef struct PhoneForward PhoneForward;

/**
 * This is the structure that stores a sequence of telephone numbers.
 */
struct PhoneNumbers;

/**
 * This is definition of structure that stores a sequence of telephone numbers.
 */
typedef struct PhoneNumbers PhoneNumbers;

/** @brief Creates a new structure.
 * Creates a new structure with no redirects.
 * @return Pointer to the created structure or NULL,
 *         when memory allocation failed.
 */
PhoneForward *phfwdNew(void);

/** @brief Deletes the structure.
 * Deletes the structure pointed to by @p pf.
 * Does nothing if the pointer is NULL.
 * @param[in] pf – pointer to the deleted structure.
 */
void phfwdDelete(PhoneForward *pf);

/** @brief Adds a redirect.
 * Adds forwarding of all numbers with a prefix @p num1, to numbers,
 * in which this prefix has been changed into a prefix accordingly @p num2.
 * Each number is its own prefix.
 * If the redirection was previously added with the same parameter @p num1,
 * so then it's replaced.
 * The number redirection relationship is not transitive.
 * @param[in,out] pf – pointer to a structure holding redirects numbers.
 * @param[in] num1   – pointer to the string representing
 *                     the prefix of numbers redirected.
 * @param[in] num2   – pointer to the string representing the prefix of numbers,
 *                     to which the redirection is made.
 * @return @p true, if a redirect has been added.
 *         @p false, if there was an error, e.g. the given string does not
 *         represent a number, both given numbers are identical, failed
 *         to allocate memory or @p pf is NULL.
 */
bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2);

/** @brief Removes redirection.
 * Removes all redirects where the parameter @p num is prefix
 * of the @p num1 parameter used when adding. If there are no such redirects,
 * string does not represent a number or @p pf is null, function does nothing.
 * @param[in,out] pf – pointer to a structure holding redirects numbers.
 * @param[in] num    – pointer to the string representing the prefix of numbers.
 */
void phfwdRemove(PhoneForward *pf, char const *num);

/** @brief Designates number redirection.
 * Designates redirection of the given number. Looks for the longest matching
 * prefix. The result is a sequence that contains at most one number. If given
 * number has not been redirected, the result is a sequence
 * containing that number. If the given string does not represent a number,
 * the result is an empty sequence. Allocates the structure @p PhoneNumbers,
 * which must be freed with @ref phnumDelete function.
 * @param[in] pf  – pointer to a structure holding redirects numbers.
 * @param[in] num – pointer to the string representing the number.
 * @return A pointer to a structure that holds a sequence of numbers,
 *         or NULL when there was memory allocation failure or @p pf is null.
 */
PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num);

/** @brief Designates redirects to a given number.
 * Designates a sequence of numbers such that: if redirection of given number
 * is a prefix of @p num, so then changes this prefix for the number,
 * that contains this redirection and then adds it the the final array.
 * Additionally final sequence always contains @p num.
 * Result numbers are sorted lexicographically and cannot be repeated.
 * If given string does not represent a phone number,
 * so then the result is empty sequence.
 * Allocates the structure @p PhoneNumbers, which must be freed with
 * function @ref phnumDelete.
 * @param[in] pf  – pointer to a structure holding redirects numbers.
 * @param[in] num – pointer to the string representing the number.
 * @return A pointer to a structure that holds a sequence of numbers,
 *         or NULL when there was memory allocation failure or @p pf is null.
 */
PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num);

/**
 * The function for given @p pf - redirect database
 * and given phone number @p num designates lexicographically sorted
 * a list of all such telephone numbers and only
 * such telephone numbers @p x, so phfwdGet(x) = num.
 * The function allocates the structure @p PhoneNumbers, which must be freed
 * using the function @ref phnumDelete. The result of the function is NULL,
 * when memory allocation failed. If the given string @p num
 * does not represent a number, the result is an empty sequence.
 * @param[in] pf - pointer to a structure holding redirects numbers.
 * @param[in] num - pointer to the string representing the number.
 * @return A pointer to a structure that holds a sequence of numbers,
 *         or NULL when there was memory allocation failure or @p pf is null.
 */
PhoneNumbers *phfwdGetReverse(PhoneForward const *pf, char const *num);

/** @brief Deletes the structure.
 * Deletes the structure pointed to by @p pnum.
 * The function does nothing is the pointer is NULL
 * @param[in] pnum – pointer to the to be deleted structure.
 */
void phnumDelete(PhoneNumbers *pnum);

/** @brief Provides a number.
 * Provides a pointer to the string representing the number.
 * The strings are indexed sequentially from zero.
 * @param[in] pnum – pointer to a structure storing a sequence of phone numbers.
 * @param[in] idx  – phone number index.
 * @return A pointer to a string representing a telephone number.
 *         NULL if @p pnum is null or index is too large.
 */
char const *phnumGet(PhoneNumbers const *pnum, size_t idx);

#endif /* __PHONE_FORWARD_H__ */