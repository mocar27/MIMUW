# ***Operating systems***

The course was intended to show how to work and develop at a very low level. There were overall five tasks that each made a contribution to the final grade.

## **Inverse permutation (Task 1 - NASM Assembly)**

The task was about to implement function *bool inverse_permutation(size_t n, int *p)** in assembly language that is later going to be called from C level programs.

The function's arguments are a pointer **p** to a non-empty array of integers and the size of that array **n**. If the array pointed to by **p** contains a permutation of numbers between 0 and n-1, the function reverses that permutation in place and the function returns **true**. Otherwise, the result of the function is **false**, and the contents of the array pointed to by **p** after the function finishes are the same as when it was called.

## **Distributed stack machine (Task 2 - NASM Assembly)**

The goal of the task was to implement a distributed stack machine simulator in x86_64 assembly language. 
The machine consists of **N** cores that are numbered from 0 to N − 1, where **N** is some constant set during the compilation of simulator.

The task was about to implement function *uint64_t core(uint64_t n, char const *p)** in assembly language that is later going to be called from C level programs.

The **n** parameter contains the core number. The **p** parameter is a pointer to the ASCIIZ string and defines the calculation to be performed by the core. 
A computation consists of operations performed on the stack.

After the core has finished performing the calculation, its result, i.e. the result of the core function, is the value from the top of the stack.

## **Payment system in MINIX (Task 3 - MINIX Modification)**

The purpose of the task was to modify MINIX operating system as to enable processes in the MINIX system to hold money and make mutual transfers.

The goal of the task was to add was to add new system call - *PM_TRANSFER_MONEY* and new library function - *int transfermoney(pid_t recipient, int amount)*.
Calling the *int transfermoney(pid_t recipient, int amount)* function should transfer the **amount** of currency units from the account of the process calling the function to the account of the process with the **recipient** ID.

## **Scheduling processes in MINIX (Task 4 - MINIX Modification)**

The goal of the task is to implement the "max before deadline" scheduling strategy in MINIX and add a system call that will enable processes to select this strategy.

Processes to be executed are scheduled in such a way as to maximize the number of processes executed before their completion date.
The implementation includes a new system function *int sched_deadline(int64_t deadline, int64_t estimate, bool kill)* allowing processes to choose a scheduling strategy.


## **Exclusive file locking by users in MINIX (Task 5 - MINIX Modification)**

The aim of the task is to extend the *vfs* server with a mechanism that allows users to block exclusive access to selected files. This mechanism will be mandatory and will operate at the level of users, not processes, and will implement temporary blocking that does not require changes to file attributes.

The file locking mechanism is based on the new *VFS_FEXCLUSIVE* and *VFS_EXCLUSIVE* system calls supported by the *vfs* server. Using them, the user can temporarily block other users from performing the following actions on a given file: opening the file, reading, writing, shortening, moving and renaming the file or deleting the file. The user who has locked the file can perform these operations without restrictions from various processes.
