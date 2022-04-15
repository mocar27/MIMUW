#include <stdio.h>
#include <stdlib.h>

#ifndef POLA
#define POLA 4
#endif

#ifndef WIERSZE
#define WIERSZE 10
#endif

#ifndef KOLUMNY
#define KOLUMNY 7
#endif

#ifndef WYBOR
#define WYBOR 234
#endif

void resetArray(int tab[], int length)
{
    for (int it = 0; it < length; it++)
        tab[it] = 0;
}
int boardEvaluation(int B[][KOLUMNY])
{
    int player = 0; // Variable holds the value of how many blocks can be put at once by the left player
    int bot = 0;    // Variable holds the value of how many blocks can be put at once by the right player

    for (int x = 0; x < WIERSZE; x++)
    {
        // Variable "length" checks whether it is possible to add a block of length and "POLA" in the line "x"
        int length = 0;

        for (int y = 0; y < KOLUMNY; y++)
        {
            if (length == POLA)
            {
                bot++;
                length = 0;
                y -= 1;
            }

            else if (B[x][y] == 0)
                length++;

            else
                length = 0;
            /* When we come across a filled place on the "BOARD", it is that in a given coherent segment of zeros
            it is unable to insert block with length "POLA" by the right player */
        }
        if (length == POLA)
            bot++;
    }

    for (int y = 0; y < KOLUMNY; y++)
    {
        // The "length" variable checks whether it is possible to add a block of length "POLA" in the "y" column
        int length = 0;

        for (int x = 0; x < WIERSZE; x++)
        {
            if (length == POLA)
            {
                player++;
                length = 0;
                x -= 1;
            }
            else if (B[x][y] == 0)
                length++;

            else
                length = 0;
            /* When we come across a filled place on the "BOARD", it is that in a given coherent segment of zeros
            it is unable to insert block with length "POLA" by the left player */
        }
        if (length == POLA)
            player++;
    }
    return bot - player; // Returns the board evaluation for the right player
}
void addingPlayersBlock(int B[][KOLUMNY], int R, int C)
{
    // Function adds to the board the block placed by the player with length "POLA" in the row "W" and column "K"
    R -= 97;
    C -= 65;
    for (int i = 0; i < POLA; i++)
    {
        B[R][C]++;
        R++;
    }
}
void addingBotsBlock(int B[][KOLUMNY], int *OP)
{
    int n = 0;                    // Variable holds the number of moves maximizing the board rating for the right player
    int result = -1000;           // Variable holds the highest board rating for the right player
    int possibilitiesRow[676];    // Array holds indexes of rows of the blocks that maximize the right player's board rating
    int possibilitiesColumn[676]; // Array holds indexes of columns of the blocks that maximize the right player's board rating
    resetArray(possibilitiesColumn, 676);
    for (int i = 0; i < WIERSZE; i++)
    {
        // (1. i 6.) "counter" variable checks if it is possible to add a block of length "POLA" in the line "i"
        int counter = 0;
        for (int j = 0; j < KOLUMNY; j++)
        {
            if (counter == POLA)
            {
                // (2.) Evaluating the board after adding a block
                int rating = boardEvaluation(B);

                // (3.) Removing the added block
                int tempK = j - POLA;
                for (int k = 0; k < POLA; k++)
                {
                    B[i][tempK]--;
                    tempK++;
                }

                // (4.) The row in which the bot added a block is i, and the column is j-POLA
                if (rating == result)
                {
                    possibilitiesRow[n] = i + 97;
                    possibilitiesColumn[n] = (j - POLA) + 65;
                    n++;
                }
                else if (rating > result)
                {
                    /* (5.) If there is a new move that maximizes the board, remove our
                    old moves that maximized board rating and throw in the current move
                    to the array of moves maximizing the board */
                    result = rating;
                    resetArray(possibilitiesRow, 676);
                    resetArray(possibilitiesColumn, 676);
                    n = 0;
                    possibilitiesRow[n] = i + 97;
                    possibilitiesColumn[n] = (j - POLA) + 65;
                    n++;
                }
                counter = 0;
                j -= POLA;
            }
            else if (B[i][j] == 0)
            {
                // If B[i][j] is a blank field on the board, it is possible that our block may fit there
                B[i][j]++;
                counter++;
            }
            else
            {
                // If we encounter 1, it means that our block will not fit the given place on the board
                int tempJ = j - counter;
                for (int k = 0; k < counter; k++)
                {
                    B[i][tempJ]--;
                    tempJ++;
                }
                counter = 0;
            }
        }
        if (counter > 0) // Condition when we get to the end of the row and our block length is > 0
        {
            if (counter == POLA)
            {
                {
                    // (2.) Evaluating the board after adding a block
                    int rating = boardEvaluation(B);

                    // (3.) Removing the added block
                    int tempK = KOLUMNY - POLA;
                    for (int k = 0; k < POLA; k++)
                    {
                        B[i][tempK]--;
                        tempK++;
                    }

                    // (4.) The row in which the bot added a block is i, and the column is j-POLA
                    if (rating == result)
                    {
                        possibilitiesRow[n] = i + 97;
                        possibilitiesColumn[n] = (KOLUMNY - POLA) + 65;
                        n++;
                    }
                    else if (rating > result)
                    {
                        /* (5.) If there is a new move that maximizes the board, remove our
                        old moves that maximized board rating and throw in the current move
                        to the array of moves maximizing the board */
                        result = rating;
                        resetArray(possibilitiesRow, 676);
                        resetArray(possibilitiesColumn, 676);
                        n = 0;
                        possibilitiesRow[n] = i + 97;
                        possibilitiesColumn[n] = (KOLUMNY - POLA) + 65;
                        n++;
                    }
                }
            }
            else
            {

                /* If we get to the end of the row and our block length "counter" > 0, but "counter" < POLA,
                then it is not possible that right player adds block at the end of the row "i" */
                int tempJ = KOLUMNY - counter;
                for (int k = 0; k < counter; k++)
                {
                    B[i][tempJ]--;
                    tempJ++;
                }
            }
        }
    }
    if (n == 0)
    {
        // (7.) Bot has given up because he can't make any move
        *OP = '.';
        printf(".");
    }
    else
    {
        // (7.) Adding a block that maximizes the board's rating
        int rBOT = possibilitiesRow[WYBOR % n];
        int cBOT = possibilitiesColumn[WYBOR % n];
        printf("%c%c\n", rBOT, cBOT);
        rBOT -= 97;
        cBOT -= 65;
        for (int i = 0; i < POLA; i++)
        {
            B[rBOT][cBOT]++;
            cBOT++;
        }
    }
    /*
        1. Check where you can add a block and add it
        2. Evaluate the board after adding the block from point "1." int rating = boardEvaluation (B);
        3. Remove the added block
        4. If rating of the board matches the current maximal board rating add the move to array of possibilities
        5. else if rating > result, so then reset array of possibilities and add the block maximizing the board rating to it
        6. Repeat for all possible block positions, then in  possibilities[] all moves that maximize the board rating will be saved
        7. If n == 0, it means that there are no options that bot can add the block, so then set *OP to '.' and exit the program,
           else add block that is in the possibilities[WYBOR%n] and print the move
    */
}

int main()
{
    // Declaration of the board with the size WIERSZE x KOLUMNY and its zeroing
    int BOARD[WIERSZE][KOLUMNY];
    for (int i = 0; i < WIERSZE; i++)
    {
        for (int j = 0; j < KOLUMNY; j++)
            BOARD[i][j] = 0;
    }

    int operator= 0; // Defines what we will do in a given move
    int row = -1;    // Determines in which row the player's block begins
    int column = -1; // Determines in which column the block was placed by the player
    while (operator!= '.')
    {
        operator= getchar();

        if (operator!= '.')
        {
            if (operator== '-')
            {
                addingBotsBlock(BOARD, &operator);

                /*  If our typed character is '-' then the player transfers the move,
                    the block from the bot's site is added and its move is printed
                    (also '.' in case of submission, then the function sets the operator to '.', prints '.' and exits the program) */
            }
            else
            {
                column = operator;
                row = getchar();

                addingPlayersBlock(BOARD, row, column);

                addingBotsBlock(BOARD, &operator);
                /*   The bot's block (which is in response to the player's move) is added and its move is printed
                (also '.' in case of surrender, then the function sets the operator to '.', prints '.' and exits the program) */
            }
        }
        getchar();
    }
}
