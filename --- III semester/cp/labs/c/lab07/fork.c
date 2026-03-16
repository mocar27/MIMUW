#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "err.h"

int main(void)
{
    printf("Hello from %d\n", getpid());

    // Fork into two processes.
    if (fork() == -1)
        syserr("Error in fork\n");

    printf("Goodbye from %d\n", getpid());

    return 0;
}
