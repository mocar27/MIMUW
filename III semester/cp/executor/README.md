# "Executor" task

The program can execute different programs to which any input data can be entered. 

When starting the task executor writes to standard output that task with precedent number has been started. 

After finishing task that was run, program writes to standard output that task with precedent number has ended with given exit code (if the task was signalled it contains the information about it).

Executor besides running tasks, can perform other commands such as:
- out T -> the command makes executor write to standard output last line that has been written by subsequent program with number T
- err T -> same command as the **out** command, but it takes last line that was written to error output by program with number T
- kill T -> commmand that fully kills the subsequent program with number T
- sleep N -> command that makes the executor sleep for N miliseconds, during this time executor doesn't take any new tasks, current tasks are not stopped
- quit -> kills executor and all of the tasks that are currently run

Files other than `executor.c` were provided by the authors of the task.
