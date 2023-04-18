#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "err.h"

#define N_PROC 5

int child(void)
{
    printf("Child: my pid is %d, my parent's pid is %d\n", getpid(), getppid());
    return 0;
}

int main(void)
{
    // Create N_PROC children.
    for (int i = 0; i < N_PROC; ++i) {
        pid_t pid;
        ASSERT_SYS_OK(pid = fork());
        if (!pid)
            return child();

        printf("Parent: my pid is %d, created child with pid %d\n", getpid(), pid);
    }

    // Wait for each child.
    for (int i = 0; i < N_PROC; ++i)
        ASSERT_SYS_OK(wait(NULL));

    return 0;
}
