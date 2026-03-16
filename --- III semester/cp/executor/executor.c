#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "err.h"

#define MAX_N_TASKS 4096
#define OUTSTRING_SIZE 1023

struct Task {
    pid_t pid;
    char* out;
    char* err;
    sem_t out_mutex;
    sem_t err_mutex;
};
typedef struct Task Task;

struct ReadingData {
    int fd;
    int taskNumber;
};
typedef struct ReadingData ReadingData;

Task tasks[MAX_N_TASKS];
int taskCounter = -1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_string_parts_destroy;

void* save_output_thread(void* data) {

    ReadingData* rd = data;
    char line[OUTSTRING_SIZE];
    FILE* fptr = fdopen(rd->fd, "r");

    while (fgets(line, OUTSTRING_SIZE, fptr)) {

        ASSERT_SYS_OK(sem_wait(&tasks[rd->taskNumber].out_mutex));

        size_t len = strlen(line);
        free(tasks[rd->taskNumber].out);
        tasks[rd->taskNumber].out = calloc(len + 1, sizeof(char));
        memcpy(tasks[rd->taskNumber].out, line, len + 1);

        if (tasks[rd->taskNumber].out == NULL || tasks[rd->taskNumber].out[len - 1] == '\n') {
            tasks[rd->taskNumber].out[len - 1] = '\0';
        }

        ASSERT_SYS_OK(sem_post(&tasks[rd->taskNumber].out_mutex));
    }
    fclose(fptr);

    return 0;
}

void* save_error_thread(void* data) {

    ReadingData* rd = data;
    char line[OUTSTRING_SIZE];
    FILE* fptr = fdopen(rd->fd, "r");

    while (fgets(line, OUTSTRING_SIZE, fptr)) {

        ASSERT_SYS_OK(sem_wait(&tasks[rd->taskNumber].err_mutex));

        size_t len = strlen(line);
        free(tasks[rd->taskNumber].err);

        tasks[rd->taskNumber].err = calloc(len + 1, sizeof(char));
        memcpy(tasks[rd->taskNumber].err, line, len + 1);

        if (tasks[rd->taskNumber].err == NULL || tasks[rd->taskNumber].err[len - 1] == '\n') {
            tasks[rd->taskNumber].err[len - 1] = '\0';
        }

        ASSERT_SYS_OK(sem_post(&tasks[rd->taskNumber].err_mutex));
    }
    fclose(fptr);

    return 0;
}

void* waiting_thread(void* data) {

    int pipe_out[2];
    int pipe_err[2];
    ASSERT_SYS_OK(pipe(pipe_out));
    ASSERT_SYS_OK(pipe(pipe_err));

    pid_t pid = fork();
    ASSERT_SYS_OK(pid);

    int status;
    char** child_program = data;

    ASSERT_SYS_OK(pthread_mutex_lock(&mutex));
    int child_task = taskCounter;
    ASSERT_SYS_OK(pthread_mutex_unlock(&mutex));

    if (pid == 0) {
        ASSERT_SYS_OK(close(pipe_out[0]));
        ASSERT_SYS_OK(close(pipe_err[0]));

        ASSERT_SYS_OK(dup2(pipe_out[1], STDOUT_FILENO));
        ASSERT_SYS_OK(dup2(pipe_err[1], STDERR_FILENO));
        ASSERT_SYS_OK(close(pipe_out[1]));
        ASSERT_SYS_OK(close(pipe_err[1]));

        ASSERT_SYS_OK(execvp(child_program[1], &child_program[1]));
    }
    else {
        tasks[child_task].pid = pid;
        printf("Task %d started: pid %d.\n", child_task, pid);

        ASSERT_SYS_OK(close(pipe_out[1]));
        ASSERT_SYS_OK(close(pipe_err[1]));

        pthread_t out_thread_id;
        pthread_t err_thread_id;

        ReadingData* output_data = calloc(1, sizeof(ReadingData));
        ReadingData* error_data = calloc(1, sizeof(ReadingData));

        output_data->fd = pipe_out[0];
        output_data->taskNumber = child_task;

        error_data->fd = pipe_err[0];
        error_data->taskNumber = child_task;

        ASSERT_SYS_OK(pthread_create(&out_thread_id, NULL, save_output_thread, output_data));
        ASSERT_SYS_OK(pthread_create(&err_thread_id, NULL, save_error_thread, error_data));

        ASSERT_SYS_OK(sem_post(&sem_string_parts_destroy));

        ASSERT_SYS_OK(pthread_join(out_thread_id, NULL));
        ASSERT_SYS_OK(pthread_join(err_thread_id, NULL));

        ASSERT_SYS_OK(wait(&status));

        ASSERT_SYS_OK(pthread_mutex_lock(&mutex));
        if (WIFEXITED(status)) {
            printf("Task %d ended: status %d.\n", child_task, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status) || WIFSTOPPED(status)) {
            if (tasks[child_task].out == NULL) {
                tasks[child_task].out = '\0';
            }
            printf("Task %d ended: signalled.\n", child_task);
        }
        free(output_data);
        free(error_data);
        ASSERT_SYS_OK(pthread_mutex_unlock(&mutex));
    }

    return 0;
}

void execRun(char** parts) {

    for (int i = 1; parts[i] != NULL; i++) {
        size_t len = strlen(parts[i]);
        if (parts[i][len - 1] == '\n') {
            parts[i][len - 1] = '\0';
        }
    }

    pthread_t thread;
    ASSERT_SYS_OK(pthread_create(&thread, NULL, waiting_thread, parts));
    ASSERT_SYS_OK(pthread_detach(thread));

    ASSERT_SYS_OK(sem_wait(&sem_string_parts_destroy));
}

void execOut(uint taskID) {
    ASSERT_SYS_OK(sem_wait(&tasks[taskID].out_mutex));
    printf("Task %d stdout: '%s'.\n", taskID, tasks[taskID].out);
    ASSERT_SYS_OK(sem_post(&tasks[taskID].out_mutex));
}

void execErr(uint taskID) {
    ASSERT_SYS_OK(sem_wait(&tasks[taskID].err_mutex));
    printf("Task %d stderr: '%s'.\n", taskID, tasks[taskID].err);
    ASSERT_SYS_OK(sem_post(&tasks[taskID].err_mutex));
}

void execQuit() {

    ASSERT_SYS_OK(pthread_mutex_lock(&mutex));
    for (int i = 0; i < taskCounter; i++) {
        if (waitpid(tasks[i].pid, NULL, WNOHANG) == 0) {
            ASSERT_SYS_OK(kill(tasks[i].pid, SIGKILL));
        }
    }
    ASSERT_SYS_OK(pthread_mutex_unlock(&mutex));
}

int main() {
    char* buffer = NULL;
    size_t size_of_buffer = 512;

    for (int i = 0; i < MAX_N_TASKS; i++) {
        ASSERT_SYS_OK(sem_init(&tasks[i].out_mutex, 1, 1));
        ASSERT_SYS_OK(sem_init(&tasks[i].err_mutex, 1, 1));
        tasks[i].out = calloc(1, sizeof(char));
        tasks[i].err = calloc(1, sizeof(char));
    }

    ASSERT_SYS_OK(sem_init(&sem_string_parts_destroy, 1, 0));

    while (getline(&buffer, &size_of_buffer, stdin) != EOF) {

        strtok(buffer, "\n");
        char** string_parts = NULL;
        string_parts = split_string(buffer);

        if (strcmp(string_parts[0], "run") == 0) {
            ASSERT_SYS_OK(pthread_mutex_lock(&mutex));
            taskCounter++;
            ASSERT_SYS_OK(pthread_mutex_unlock(&mutex));
            execRun(string_parts);
        }
        else if (strcmp(string_parts[0], "out") == 0) {
            uint tID = (uint)strtol(string_parts[1], NULL, 10);
            execOut(tID);
        }
        else if (strcmp(string_parts[0], "err") == 0) {
            uint tID = (uint)strtol(string_parts[1], NULL, 10);
            execErr(tID);
        }
        else if (strcmp(string_parts[0], "kill") == 0) {
            uint tID = (uint)strtol(string_parts[1], NULL, 10);

            if (waitpid(tasks[tID].pid, NULL, WNOHANG) == 0) {
                ASSERT_SYS_OK(kill(tasks[tID].pid, SIGINT));
            }
        }
        else if (strcmp(string_parts[0], "sleep") == 0) {
            ulong sleepTimer = (ulong)strtol(string_parts[1], NULL, 10);
            usleep(sleepTimer * 1000);
        }
        else if (strcmp(string_parts[0], "quit") == 0) {
            execQuit();
        }

        free(buffer);
        buffer = NULL;
        free_split_string(string_parts);
    }

    execQuit();
    for (int i = 0; i < MAX_N_TASKS; i++) {
        ASSERT_SYS_OK(sem_destroy(&tasks[i].out_mutex));
        ASSERT_SYS_OK(sem_destroy(&tasks[i].err_mutex));
        free(tasks[i].out);
        free(tasks[i].err);
    }
    ASSERT_SYS_OK(sem_destroy(&sem_string_parts_destroy));
    free(buffer);

    return 0;
}