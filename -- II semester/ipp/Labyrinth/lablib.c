#include "strct.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

size_t fastMultiply(size_t x, size_t y) {
    size_t power = x;
    for (size_t i = 1; i < y; i++)
        x *= power;
    return x;
}

void findAWayOut(size_t **n, size_t **x, size_t **y, size_t k, size_t sizeOfMaze, Bitset *bitset) {
    int noWay = 1;
    size_t result = 0;
    size_t start = 0;
    size_t end = 0;
    size_t dim = 1;
    size_t finish = 0;
    size_t previous = 0;

    for (size_t i = 1; i <= k; i++) {
        start += ((*x)[i] - 1) * dim;
        end += ((*y)[i] - 1) * dim;
        dim *= (*n)[i];
    }
    if (start == end) {
        noWay = 0;
    } else {
        Queue *queue = createQueue();
        size_t *dimensions = calloc(k + 1, sizeof(size_t));
        dimensions[0] = 1;

        for (size_t i = 1; i <= k; i++)
            dimensions[i] = dimensions[i - 1] * (*n)[i];

        push(queue, start);

        size_t currectBit = start;

        while (!empty(queue) && currectBit != end && !finish) {
            currectBit = pop(queue);

            addBitset(bitset, currectBit);

            for (size_t i = 0; i < k; i++) {
                if (currectBit >= dimensions[i] && currectBit - dimensions[i] <= sizeOfMaze) {

                    if ((currectBit - dimensions[i]) / dimensions[i + 1] == currectBit / dimensions[i + 1]) {
                        if (!findBitset(bitset, currectBit - dimensions[i])) {
                            addBitset(bitset, currectBit - dimensions[i]);
                            push(queue, currectBit - dimensions[i]);
                        }
                    }
                }
                if (currectBit + dimensions[i] <= sizeOfMaze) {

                    if ((currectBit + dimensions[i]) / dimensions[i + 1] == currectBit / dimensions[i + 1]) {
                        if (!findBitset(bitset, currectBit + dimensions[i])) {
                            addBitset(bitset, currectBit + dimensions[i]);
                            push(queue, currectBit + dimensions[i]);
                        }
                    }
                }
            }

            if (currectBit == start) {
                if (previous == start) {
                    finish = 1;
                    result--;
                }
                result++;
                push(queue, start);
            }
            previous = currectBit;
        }

        while (!empty(queue))
            currectBit = pop(queue);
        free(queue);
        free(dimensions);
    }
    if (result != 0 && findBitset(bitset, end))
        noWay = 0;

    noWay ? fprintf(stdout, "NO WAY\n") : fprintf(stdout, "%lu\n", result);
}