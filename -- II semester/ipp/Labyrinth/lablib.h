#ifndef _LABLIB_H
#define _LABLIB_H

typedef struct Bitset Bitset;
#include <stddef.h>
#include "strct.h"

// Library contains declaration of functions that searches labyrinth

size_t fastMultiply(size_t, size_t);    // function that takes x and returns x to the power of y

void findAWayOut(size_t **, size_t **, size_t **, size_t , size_t , Bitset *); // functions that search for the way out in labyrinth

#endif
