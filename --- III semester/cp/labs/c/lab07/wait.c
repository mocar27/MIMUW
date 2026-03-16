#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "err.h"

int main(void)
{
    pid_t pid;
    ASSERT_SYS_OK(pid = fork());
    if (pid == 0) {
        // Child process
        printf("Child: my pid is %d, my parent's pid is %d\n", getpid(), getppid());
        printf("Child: fork() returned %d\n", pid);
        return 0;
    } else {
        // Parent process
        printf("Parent: my pid is %d\n", getpid());
        printf("Parent: fork() returned %d\n", pid);

        // Wait for child.
        ASSERT_SYS_OK(wait(NULL));

        return 0;
    }
}
