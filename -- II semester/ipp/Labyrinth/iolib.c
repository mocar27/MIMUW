#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>
#include "iolib.h"
#include "prolib.h"
#include "strct.h"
#include "lablib.h"

void printError(int *line, bool *problem) {

    if (!(*problem)) {
        fprintf(stderr, "ERROR %d\n", *line);
        (*problem) = true;
    }
}

bool safeAlloc(size_t **tab) {
    if (*tab == NULL)
        return false;
    else
        return true;
}

void safeFree(size_t **tab) {
    if (*tab == NULL)
        return;
    free(*tab);
}

void extendMemory(size_t **tab, size_t *extend, bool *problem, int *line) {
    (*extend) *= 2;
    *tab = realloc(*tab, ((*extend) + 1) * sizeof(SIZE_MAX));

    if (!safeAlloc(tab)) {
        (*line) = 0;
        printError(line, problem);
    }
}

int isEOF() {
    return feof(stdin);
}

void read(size_t **tab, size_t *k, bool *problem, int *line) {

    (*line)++;
    (*k) = 1;

    size_t extend = 100;

    int character = 0;
    int *temp = calloc(100, sizeof(int));
    int iterator = 0;
    size_t sum = 0;
    bool number = false;

    if (!safeAlloc(tab)) {
        (*line) = 0;
        printError(line, problem);
    }

    if (isEOF())
        printError(line, problem);

    while (!(*problem) && character != '\n') {
        character = getchar();

        while (!(*problem) && !isspace(character)) {
            if (iterator > 20)
                printError(line, problem);

            if (character < '0' || character > '9')
                printError(line, problem);

            else if ((iterator != 0 && character >= '0') || (iterator == 0 && character > '0')) {

                temp[iterator] = character - '0';
                iterator++;
            }
            number = true;
            character = getchar();
            if (isEOF()) {
                (*line)++;
                printError(line, problem);
            }
        }

        if (number == true) {
            saveNumberULL(&temp, &iterator, &sum, problem, line);

            if (sum == 0)
                printError(line, problem);
            else {
                (*tab)[*k] = sum;
                (*k)++;
            }

            iterator = 0;
            sum = 0;
            number = false;
        }

        if ((*k) == extend)
            extendMemory(tab, &extend, problem, line);
    }

    *tab = realloc(*tab, (*k) * sizeof(size_t));
    (*k)--;
    free(temp);

    if (character == '\n' && (*k) == 0) {
        printError(line, problem);
    } else if (!safeAlloc(tab)) {
        (*line) = 0;
        printError(line, problem);
    }
}

int determinateStyle(bool *problem, int *line) {

    (*line)++;
    int character = 0;
    int readingStyle = -1;

    while (!(*problem) && readingStyle == -1) {
        character = getchar();

        if (character == '\n' || isEOF()) {
            printError(line, problem);
        } else if (!isspace(character) && character != '0' && character != 'R') {
            printError(line, problem);
        } else if (character == 'R') {
            readingStyle = 1;
        } else if (character == '0') {
            character = getchar();

            if (character == 'x')
                readingStyle = 0;

            else {
                printError(line, problem);
            }
        }
    }
    return readingStyle;
}

void readFourthLineR(Uint *a, Uint *b, Uint *m, Uint *r, Uint *s0, bool *problem, int *line) {

    int character = 0;
    Uint *tab = calloc(5, sizeof(Uint));
    int liczba = 0;

    Uint sum = 0;
    int iterator = 0;
    int *temp = calloc(100, sizeof(int));
    bool number = false;

    while (!(*problem) && character != '\n' && character != EOF) {

        if (isEOF() && liczba < 4)
            printError(line, problem);

        character = getchar();

        while (!(*problem) && !isspace(character) && character != EOF) {
            if (character < '0' || character > '9')
                printError(line, problem);

            else if ((iterator != 0 && character >= '0') || (iterator == 0 && character > '0')) {
                temp[iterator] = character - '0';
                iterator++;
            }
            number = true;
            character = getchar();
        }

        if (number == true) {
            saveNumberUINT(&temp, &iterator, &sum, problem, line);

            if (liczba <= 4) {
                tab[liczba] = sum;
                liczba++;
            } else {
                printError(line, problem);
            }

            iterator = 0;
            sum = 0;
            number = false;
        }
    }
    if (tab[2] == 0)
        printError(line, problem);

    (*a) = tab[0];
    (*b) = tab[1];
    (*m) = tab[2];
    (*r) = tab[3];
    (*s0) = tab[4];

    free(tab);
    free(temp);

    if (character == '\n' && liczba < 4)
        printError(line, problem);

}

void fillBitsetR(Bitset *bitset, Uint a, Uint b, Uint m, Uint r, Uint s, size_t sizeOfMaze) {

    size_t modulo = fastMultiply(2, 32);
    size_t w = 0;
    unsigned long int test;
    size_t sT = (size_t) s;
    size_t aT = (size_t) a;
    size_t bT = (size_t) b;
    size_t mT = (size_t) m;
    for (Uint i = 1; i <= r; i++) {
        sT = (aT * sT + bT) % mT;
        w = sT % sizeOfMaze;
        addBitset(bitset, w);

        while (!__builtin_uaddl_overflow(w, modulo, &test) && w + modulo <= sizeOfMaze) {
            w += modulo;
            addBitset(bitset, w);
        }
    }
}

void fillBitsetHexadecimal(Bitset *bitset, size_t sizeOfMaze, bool *problem, int *line) {
    int character = 0;
    size_t leadingZeros = 0;
    size_t numberOfCharacters = 0;
    size_t number;
    size_t size = 0;

    character = getchar();
    while (character == '0') {
        character = getchar();
        leadingZeros++;
        numberOfCharacters++;
    }

    ungetc(character, stdin);

    if (sizeOfMaze > 3)
        number = (sizeOfMaze - 1);
    else
        number = 3;
    while (!(*problem) && character != '\n') {
        character = getchar();

        if (character >= '0' && character <= '9') {
            character -= '0';
            size++;
            numberOfCharacters++;
        } else if (character >= 'A' && character <= 'Z') {
            character -= 'A';
            character += 10;
            size++;
            numberOfCharacters++;
        } else if (character >= 'a' && character <= 'z') {
            character -= 'a';
            character += 10;
            size++;
            numberOfCharacters++;
        } else if (character != '\n') {
            if (size > 0 || leadingZeros > 0) {
                while (isspace(character) && character != '\n')
                    character = getchar();
                if (character != '\n')
                    printError(line, problem);
            } else
                printError(line, problem);
        }

        if (number == 0 && character != '\n')
            printError(line, problem);

        if (numberOfCharacters - leadingZeros > size)
            printError(line, problem);

        if (!(*problem) && character != '\n')
            switch (character) {
                case 1:
                    addBitset(bitset, number - 3);
                    break;
                case 2:
                    addBitset(bitset, number - 2);
                    break;
                case 3:
                    addBitset(bitset, number - 3);
                    addBitset(bitset, number - 2);
                    break;
                case 4:
                    addBitset(bitset, number - 1);
                    break;
                case 5:
                    addBitset(bitset, number - 3);
                    addBitset(bitset, number - 1);
                    break;
                case 6:
                    addBitset(bitset, number - 2);
                    addBitset(bitset, number - 1);
                    break;
                case 7:
                    addBitset(bitset, number - 3);
                    addBitset(bitset, number - 2);
                    addBitset(bitset, number - 1);
                    break;
                case 8:
                    addBitset(bitset, number);
                    break;
                case 9:
                    addBitset(bitset, number - 3);
                    addBitset(bitset, number);
                    break;
                case 10:
                    addBitset(bitset, number - 2);
                    addBitset(bitset, number);
                    break;
                case 11:
                    addBitset(bitset, number - 3);
                    addBitset(bitset, number - 2);
                    addBitset(bitset, number);
                    break;
                case 12:
                    addBitset(bitset, number - 1);
                    addBitset(bitset, number);
                    break;
                case 13:
                    addBitset(bitset, number - 3);
                    addBitset(bitset, number - 1);
                    addBitset(bitset, number);
                    break;
                case 14:
                    addBitset(bitset, number - 2);
                    addBitset(bitset, number - 1);
                    addBitset(bitset, number);
                    break;
                case 15:
                    addBitset(bitset, number - 3);
                    addBitset(bitset, number - 2);
                    addBitset(bitset, number - 1);
                    addBitset(bitset, number);
                    break;
            }

        if (number >= 4)
            number -= 4;
        else
            number = 0;
    }
    if (!size && !leadingZeros)
        printError(line, problem);

    if (sizeOfMaze < numberOfCharacters - leadingZeros)
        printError(line, problem);

    (*line)++;

}



