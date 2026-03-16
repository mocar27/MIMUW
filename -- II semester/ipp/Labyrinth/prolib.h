#ifndef _PROLIB_H
#define _PROLIB_H
#define Uint unsigned int
typedef struct Bitset Bitset;

#include <stddef.h>
#include "iolib.h"
#include "prolib.h"
#include "strct.h"
#include "lablib.h"

// Library contains declaration of functions checking whether input is correct

void checkDimension(size_t **, size_t **, size_t, size_t, bool *, int *);   // check each dimension

size_t checkSizeOfLabyrinth(size_t **, size_t, bool *, int *);  // check if size of labyrinth is smaller than SIZE_MAX

void saveNumberULL(int **, int *, size_t *, bool *, int *); // saves number that is unsigned long long int

void saveNumberUINT(int **, int *, Uint *, bool *, int *);  // saves number that is unsigned int

void checkPositions(size_t **, size_t **, size_t **, size_t, Bitset *, bool *, int *);  // check whether position of start or end is empty

#endif
