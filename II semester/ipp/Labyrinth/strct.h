#ifndef _STRCT_H
#define _STRCT_H
#include <stddef.h>
#include "iolib.h"
#include "prolib.h"
#include "strct.h"
#include "lablib.h"


// Structures
struct node;
struct queue;
struct Bitset;

// Definitions of structs
typedef struct node newNode;
typedef struct queue Queue;
typedef struct Bitset Bitset;


// Queue functions
Queue *createQueue();   // creates queue

int empty(Queue *);     // returns 0 or 1 depending on whether queue is empty or not

void push(Queue *, size_t); // pushes value to queue

size_t pop(Queue *);    // take the first value off the queue

// Bitset functions
Bitset *createBitset(size_t);   // creates bitset

void addBitset(Bitset *, size_t);   // adds number to bitset

bool findBitset(Bitset *, size_t);  // searches for given number in the bitset

void deleteBitset(Bitset **);   // deletes bitset

#endif
