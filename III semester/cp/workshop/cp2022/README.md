# "Workshop" task

The program creates, depending on the input data, workshops with stations inside them.

After creating the workshops and stations, the program creates threads that will later enter workshops and stations they want.

Threads occupy stations concurrently, they can enter the workshop, enter the station, leave current place of occupation or switch seats.

Thread can switch seats inside one workshop, it can do it instatly if the seat is not taken, otherwise the thread must wait until seat will be free.

Problem of switching threads in cycles is solved has been solved in the implementation of this task. 

The program is fully written in Java using all of the mechanisms to write concurrent programs:
- Semaphores
- CountDownLatch
- Theads
- Mutual exclusion data access

Files other than in the `solution` directory were provided by the author of the task.
