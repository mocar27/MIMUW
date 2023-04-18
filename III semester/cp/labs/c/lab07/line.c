#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "err.h"

#define N_PROC 5

int child(void) {
    printf("Child: my pid is %d, my parent's pid is %d\n", getpid(), getppid());
    return 0;
}

int main(int argc, char* argv[]) {

    int n;

    if (argc < 2) {
        n = N_PROC;
    }
    else {
        n = atoi(argv[1]);
    }

    char buffer[10];
    int result = snprintf(buffer, sizeof buffer, "%d", n - 1);
    if (result < 0 || result >= (int)sizeof(buffer)) {
        fatal("snprintf failed");
    }

    if (n != 0) {

        pid_t pid;
        ASSERT_SYS_OK(pid = fork());

        if (!pid) {
            child();
            ASSERT_SYS_OK(execlp("./line", "line", buffer, NULL));
        }
        else {
            ASSERT_SYS_OK(wait(NULL));
        }
    }
    return 0;
}