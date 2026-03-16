#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include "iolib.h"
#include "prolib.h"
#include "strct.h"
#include "lablib.h"
#define Uint unsigned int

int main(void) {

    size_t *n = calloc(101, sizeof(size_t));
    size_t *x = calloc(101, sizeof(size_t));
    size_t *y = calloc(101, sizeof(size_t));
    size_t kn, kx, ky;

    bool problem = false;
    int line = 0;

    read(&n, &kn, &problem, &line);
    size_t sizeOfLabyrinth = checkSizeOfLabyrinth(&n, kn, &problem, &line);

    if (!problem) {
        read(&x, &kx, &problem, &line);
        checkDimension(&x, &n, kx, kn, &problem, &line);
    }

    if (!problem) {
        read(&y, &ky, &problem, &line);
        checkDimension(&y, &n, ky, kn, &problem, &line);
    }

    int readingFourthLine = determinateStyle(&problem, &line);
    Bitset *bitset = createBitset(sizeOfLabyrinth);

    if (!problem && !readingFourthLine) {
        fillBitsetHexadecimal(bitset, sizeOfLabyrinth, &problem, &line);
    } else if (!problem && readingFourthLine) {

        Uint a, b, m, r, s0;
        readFourthLineR(&a, &b, &m, &r, &s0, &problem, &line);

        if (!problem)
            fillBitsetR(bitset, a, b, m, r, s0, sizeOfLabyrinth);
    }
    if (!problem)
        checkPositions(&n, &x, &y, kn, bitset, &problem, &line);

    if (!problem)
        findAWayOut(&n, &x, &y, kn, sizeOfLabyrinth, bitset);

    deleteBitset(&bitset);
    safeFree(&n);
    safeFree(&x);
    safeFree(&y);

    return problem;
}