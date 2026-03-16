#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "pipeline-utils.h"
#include "err.h"

int main(int argc, char** argv) {

    argc < 2 ? fatal("Usage: %s <command> [<args>]", argv[0]) : 0;

    int pipe_dsc[2];
    ASSERT_SYS_OK(pipe(pipe_dsc));

    char* command = argv[1];

    pid_t pid = fork();
    ASSERT_SYS_OK(pid);

    int i = 0;
    if (!pid) {
        for (i = 1; i < argc - 1 && pid == 0; i++) {
            ASSERT_SYS_OK(close(pipe_dsc[1]));
            ASSERT_SYS_OK(dup2(pipe_dsc[0], STDIN_FILENO));
            ASSERT_SYS_OK(close(pipe_dsc[0]));

            ASSERT_SYS_OK(pipe(pipe_dsc));

            command = argv[i + 1];

            if (i < argc - 2) {
                pid = fork();
                ASSERT_SYS_OK(pid);
            }
        }
    }

    ASSERT_SYS_OK(close(pipe_dsc[0]));
    if (i < argc - 1) {
        ASSERT_SYS_OK(dup2(pipe_dsc[1], STDOUT_FILENO));
    }
    ASSERT_SYS_OK(close(pipe_dsc[1]));

    ASSERT_SYS_OK(execlp(command, command, NULL));
}