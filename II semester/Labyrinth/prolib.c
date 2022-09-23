#include <stdbool.h>
#include <stddef.h>
#include "iolib.h"
#include "prolib.h"
#include "strct.h"
#include "lablib.h"
void checkDimension(size_t **tab1, size_t **tab2, size_t size1, size_t size2, bool *problem, int *line) {
    if (size1 != size2)
        printError(line, problem);
    else
        for (size_t j = 1; j <= size2; j++) {
            if ((*tab1)[j] > (*tab2)[j])
                printError(line, problem);
        }
}

size_t checkSizeOfLabyrinth(size_t **tab, size_t size, bool *problem, int *line) {
    unsigned long long int multiplication;
    size_t dimensions = 1;
    for (size_t j = 1; j <= size; j++) {
        size_t x = (*tab)[j];
        if (!__builtin_umulll_overflow(dimensions, x, &multiplication))
            dimensions *= x;
        else {
            (*line) = 0;
            printError(line, problem);
        }
    }
    return dimensions;
}

void saveNumberULL(int **tab, int *length, size_t *sum, bool *problem, int *line) {
    size_t power = 1;
    unsigned long long int test;

    if ((*length) > 0) {
        (*sum) = 0;
        for (int it = (*length) - 1; it >= 0; it--) {
            size_t x = (size_t) (*tab)[it];
            if (!__builtin_umulll_overflow(x, power, &test)) {
                if (!__builtin_uaddll_overflow((*sum), x * power, &test)) {
                    (*sum) += x * power;
                    power *= 10;
                    (*tab)[it] = 0;
                } else {
                    printError(line, problem);
                }
            } else {
                printError(line, problem);
            }
        }
    }
}

void saveNumberUINT(int **tab, int *length, Uint *sum, bool *problem, int *line) {

    Uint power = 1;
    Uint test;

    if ((*length) > 10)
        printError(line, problem);

    else if ((*length) > 0) {
        (*sum) = 0;
        for (int it = (*length) - 1; it >= 0; it--) {
            Uint x = (Uint) (*tab)[it];
            if (!__builtin_umul_overflow(x, power, &test)) {
                if (!__builtin_uadd_overflow((*sum), x * power, &test)) {
                    (*sum) += x * power;
                    power *= 10;
                    (*tab)[it] = 0;
                } else {

                    printError(line, problem);
                }
            } else {

                printError(line, problem);
            }
        }
    }
}

void checkPositions(size_t **n, size_t **x, size_t **y, size_t k, Bitset *bitset, bool *problem, int *line) {
    size_t bitX = 0;
    size_t bitY = 0;
    size_t dim = 1;
    for (size_t i = 1; i <= k; i++) {
        bitX += ((*x)[i] - 1) * dim;
        bitY += ((*y)[i] - 1) * dim;
        dim *= (*n)[i];
    }

    if (findBitset(bitset, bitX)) {
        (*line) = 2;
        printError(line, problem);
    } else if (findBitset(bitset, bitY)) {
        (*line) = 3;
        printError(line, problem);
    }
}