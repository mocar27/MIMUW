#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include "iolib.h"
#include "prolib.h"
#include "strct.h"
#include "lablib.h"
struct node {
    size_t value; // value kept in struct
    struct node *next;  // pointer to another same-type structure
};

struct queue {
    newNode *first, *last;  // pointers to first and last element of queue
};

struct Bitset {
    size_t maxVal;  // maximal value in bitset
    size_t *bitTable;   // table of bits
};

// Queue functions
Queue *createQueue() {
    Queue *q = malloc(sizeof(Queue));
    if (q == NULL)
        return NULL;
    q->first = NULL;
    q->last = NULL;
    return q;
}

int empty(Queue *q) {
    if (q->first == NULL)
        return 1;
    else
        return 0;
}

void push(Queue *q, size_t x) {
    newNode *temp = calloc(1, sizeof(newNode));

    temp->value = x;
    temp->next = NULL;
    if (q->first == NULL) {
        q->first = temp;
        q->last = temp;
    } else {
        q->last->next = temp;
        q->last = temp;
    }
}

size_t pop(Queue *q) {
    newNode *pom = q->first;
    size_t v = q->first->value;
    if (q->first == q->last)
        q->first = NULL;
    else
        q->first = q->first->next;
    if (q->first == NULL)
        q->last = NULL;
    free(pom);
    return v;
}

// Bitset functions
Bitset *createBitset(size_t number) {

    Bitset *newBitset = calloc(1, sizeof(Bitset));

    if (newBitset == NULL)
        return NULL;

    newBitset->maxVal = number;
    size_t tempSize = number / 64 + 1;

    size_t *tempTable = calloc(tempSize, sizeof(size_t));
    if (tempTable == NULL) {
        free(newBitset);
        return NULL;
    }

    newBitset->bitTable = tempTable;

    return newBitset;
}

void addBitset(Bitset *bitset, size_t number) {
    if (bitset == NULL)
        return;

    size_t tempBit = (1U << (number % 64));
    bitset->bitTable[number / 64] |= tempBit;
}

bool findBitset(Bitset *bitset, size_t number) {
    if (bitset == NULL)
        return NULL;

    size_t tempBit = (1U << (number % 64));
    size_t tempMask = (bitset->bitTable[number / 64]);

    return (bool) (tempBit & tempMask);
}

void deleteBitset(Bitset **bitset) {
    if (*bitset == NULL)
        return;

    free((*bitset)->bitTable);
    free(*bitset);
}

