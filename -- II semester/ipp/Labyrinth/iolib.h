#ifndef _IOLIB_H
#define _IOLIB_H
#define Uint unsigned int

typedef struct Bitset Bitset;
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include "strct.h"
// Library contains declaration of functions checking whether data that is being entered is correct

void printError(int *, bool *); // prints error

bool safeAlloc(size_t **);  // check whether malloc / calloc / realloc was successful

void safeFree(size_t **);   // frees allocated structure if it's not NULL

void extendMemory(size_t **, size_t *, bool *, int *);  // extend memory needed logarithmically

int isEOF();    // returns wheter it's end of file

void read(size_t **, size_t *, bool *, int *);  // reads first three lines of input

int determinateStyle(bool *, int *);    // determinates whether fourth line is given in "R-style" or "0x-style"

void readFourthLineR(Uint *, Uint *, Uint *, Uint *, Uint *, bool *, int *);    // reads fourth line

void fillBitsetR(Bitset *, Uint, Uint, Uint, Uint, Uint, size_t);   // fills bitset when 4th line is given in "R-style"

void fillBitsetHexadecimal(Bitset *, size_t, bool *, int *);    // reads and fills bitset when 4th line is given in "0x-style"

#endif
