#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
void resetArray(int tab[], int n)
{
    for (int it = 0; it < n; it++)
        tab[it] = 0;
}
void addition(int w[], int l[])
{
    // to the coefficients of the polynomial "w" we add the coefficients of the polynomial "l"
    for (int it = 0; it < 11; it++)
        w[it] += l[it];
}
void multiplication(int w[], int l[])
{
    int temporary[11]; // temporary array in which the current polynomial will be held
    for (int it = 10; it >= 0; it--)
        temporary[it] = w[it];

    resetArray(w, 11);
    for (int it = 10; it >= 0; it--)
    {
        /* if we multiply "* 0" then the result will also be correct, because we will not enter this condition even once,
        and our array to which we want to enter the result of multiplication is zeroed*/
        if (l[it] != 0)
        {
            for (int it1 = 10; it1 >= 0; it1--)
            {
                if (it + it1 <= 10)
                    w[it1 + it] += temporary[it1] * l[it];
                /* we put the result of multiplication by monomials whose coefficients are different from 0 of our old polynomial
                into the zeroed array "w" */
            }
        }
    }
}
// only triggers for the first monomial in the data line, because we don't know whether data starts with '+', '+ -', '*', '* -'
void firstConversion(int num[], int temp[], int power, int itr)
{
    if (itr == 0) // when itr == 0, so there is nothing in the first position in the temporary array and the operator is '+'
        num[power]++;

    else if (itr == 1) // when itr == 1, so on the first position in the temporary array is '-' or value in range 2 to 9
    {
        if (temp[0] == -3) // when the first position is minus & itr == 1, we add -1
            num[power]--;

        else // on the first position we have value in range 2 to 9 and so we add it to the coefficien of monomial x^power
            num[power] += temp[0];
    }

    else
    {
        int result = 0;
        if (temp[0] == -3)
        { // converting from an array of digits to one number (coefficient of monomial x ^ power) when it starts with '-'
            for (int j = 1; j < itr; j++)
            {
                if (temp[j] != -1)
                {
                    if (result != 0)
                    {
                        result *= 10;
                        result -= temp[j];
                    }
                    else
                        result -= temp[j];
                }
            }
            num[power] += result;
        }

        else
        { // converting from an array of digits to one number (coefficient of monomial x ^ power) when it starts with '+'
            for (int j = 0; j < itr; j++)
            {
                if (temp[j] != -1)
                {
                    if (result != 0)
                    {
                        result *= 10;
                        result += temp[j];
                    }
                    else
                        result += temp[j];
                }
            }
            num[power] += result;
        }
    }
    for (int j = 0; j < 11; j++)
        temp[j] = -1;
}
// the function conversionPlus and firstConversion change a number from the array "temp" to one coefficient of the monomial
void conversionPlus(int num[], int temp[], int power, int itr)
{
    if (itr == 1) // when itr == 1, the first position in the temporary array is '+' or '-'
    {
        if (temp[0] == -3)
            num[power]--;
        else
            num[power]++;
    }
    else
    { // conversion from an array of digits to one number (monomial coefficient x ^ power)
        int result = 0;
        if (temp[0] == -3)
        { // conversion to one number, when the first value in array is '-'
            for (int j = 1; j < itr; j++)
            {
                if (temp[j] != -1)
                {
                    if (result != 0)
                    {
                        result *= 10;
                        result -= temp[j];
                    }
                    else
                        result -= temp[j];
                }
            }
            num[power] += result;
        }

        else
        { // conversion to one number, when the first value in array is '+'
            for (int j = 1; j < itr; j++)
            {
                if (temp[j] != -1)
                {
                    if (result != 0)
                    {
                        result *= 10;
                        result += temp[j];
                    }
                    else
                        result += temp[j];
                }
            }
            num[power] += result;
        }
    }
    for (int j = 0; j < 11; j++)
        temp[j] = -1;
}
void conversion(int p[], int l[], int id)
{
    int temporary[11];
    for (int j = 0; j < 11; j++)
        temporary[j] = -1;

    int it = 0;
    int toPower = 0;
    int iterator = 0;
    int first = 0;
    /* we iterate over the array with the data,
    to convert all monomial coefficients from char values ​​to int */
    while (it < id)
    {
        toPower = 0;
        if (p[it] == 'x')
        {
            toPower = 1;
            it++;
            if (p[it] == '^')
            {
                it++;
                if (p[it] == '1') // the coefficient of the monomial standing at x ^ 10
                {
                    toPower = 10;
                    it += 2;
                    if (first == 0)
                    {
                        first = 1;
                        firstConversion(l, temporary, toPower, iterator);
                    }
                    else
                        conversionPlus(l, temporary, toPower, iterator);
                    iterator = 0;
                }
                else // the coefficient of the monomial standing with the powers x from 2 to 9
                {
                    toPower = p[it] - 48;
                    it++;
                    if (first == 0)
                    {
                        first = 1;
                        firstConversion(l, temporary, toPower, iterator);
                    }
                    else
                        conversionPlus(l, temporary, toPower, iterator);
                    iterator = 0;
                }
            }
            else // the coefficient of the monomial standing at x ^ 1
            {
                if (first == 0)
                {
                    first = 1;
                    firstConversion(l, temporary, toPower, iterator);
                }
                else
                    conversionPlus(l, temporary, toPower, iterator);
                iterator = 0;
            }
        }
        else
        {
            /* as long as there is no "x" in the array with the data,
            then we put it into the array to then convert it to the monomial coefficient */
            if (p[it] != '\n')
            {
                temporary[iterator] = p[it] - 48;
                iterator++;
            }

            it++;
        }
    }
    if (toPower == 0) // the coefficient of the monomial standing at x ^ 0
    {
        if (first == 0)
        {
            first = 1;
            firstConversion(l, temporary, toPower, iterator);
        }
        else
            conversionPlus(l, temporary, toPower, iterator);
        iterator = 0;
    }
}
void wypisz(int w[])
{
    int printed = 0;
    int first = 0;
    for (int it = 10; it >= 0; it--)
    {
        if (w[it] != 0)
        {
            printed = 1;    // means we've already printed anything
            if (first == 0) // printing when nothing has been printed yet
            {
                first = 1;
                if (it == 1 && w[it] == -1)
                    printf("-x");
                else if (it == 1 && w[it] == 1)
                    printf("x");
                else if (it == 1 && w[it] == INT_MIN)
                    printf("-2147483648x");
                else if (it == 1 && w[it] < 0)
                    printf("-%dx", (-1) * w[it]);
                else if (it == 1 && w[it] > 0)
                    printf("%dx", w[it]);
                else if (it == 0 && w[it] == INT_MIN)
                    printf("-2147483648");
                else if (it == 0 && w[it] < 0)
                    printf("-%d", (-1) * w[it]);
                else if (it == 0 && w[it] > 0)
                    printf("%d", w[it]);
                else if (w[it] == -1)
                    printf("-x^%d", it);
                else if (w[it] == 1)
                    printf("x^%d", it);
                else if (w[it] == INT_MIN)
                    printf("-2147483648x^%d", it);
                else if (w[it] < 0)
                    printf("-%dx^%d", (-1) * w[it], it);
                else
                    printf("%dx^%d", w[it], it);
            }
            else if (it == 1) // printing when we get to x ^ 1
            {
                if (w[it] == -1)
                    printf(" - x");
                else if (w[it] == 1)
                    printf(" + x");
                else if (w[it] == INT_MIN)
                    printf(" - 2147483648x");
                else if (w[it] < 0)
                    printf(" - %dx", (-1) * w[it]);
                else
                    printf(" + %dx", w[it]);
            }
            else if (it == 0) // printing when we get to x ^ 0
            {
                if (w[it] == INT_MIN)
                    printf(" - 2147483648");
                else if (w[it] < 0)
                    printf(" - %d", (-1) * w[it]);
                else
                    printf(" + %d", w[it]);
            }
            else // printing when (power x) = / = 0 and (power x) = / = 1
            {
                if (w[it] == -1)
                    printf(" - x^%d", it);
                else if (w[it] == 1)
                    printf(" + x^%d", it);
                else if (w[it] == INT_MIN)
                    printf(" - 2147483648x^%d", it);
                else if (w[it] < 0)
                    printf(" - %dx^%d", (-1) * w[it], it);
                else
                    printf(" + %dx^%d", w[it], it);
            }
        }
    }
    if (printed == 0) // when we have not printed anything and we have come to the end, we print 0
        printf("0");
}

int main()
{
    int coefficients[11]; // the table contains all the coefficients of the polynomial
    int task[1001];       // the array contains the row of data (no spaces) (array of char values)
    int numbers[11];      // the array holds the polynomial given in the data line with the coefficients converted into variables of the int type ('char' - 48)

    resetArray(coefficients, 11);
    resetArray(numbers, 11);

    int operator= 0; // defines what action will be performed: addition, multiplication or program termination
    while (operator!= '.')
    {

        int index = 0; // control index in the "task" array
        int c = 0;
        for (int i = 0; i < 1001; i++)
            task[i] = 0;

        operator= getchar();
        while (c != '\n') // typing the entire line of data into the "task" array, not including spaces
        {
            c = getchar();
            if (c != ' ' && c != '.')
            {
                task[index] = c;
                index++;
            }
        }

        if (operator== '+')
        {
            conversion(task, numbers, index);
            addition(coefficients, numbers);
        }

        else if (operator== '*')
        {
            conversion(task, numbers, index);
            multiplication(coefficients, numbers);
        }
        /* after performing the "+" or "*" operation, we reset the array in which the polynomial coefficients were kept from the command,
         so that it does not turn out that the next time we add or multiply by a monomial which was not in the data line*/
        resetArray(numbers, 11);
        if (operator!= '.')
        {
            wypisz(coefficients); // prints the polynomial after performing the operations specified on the line
            printf("\n");
        }
    }
}