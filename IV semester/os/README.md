# ***Operating systems***

The course was intended to show how to work and program at a very low level. There were overall five tasks that each made a contribution to the final grade.

## **Inverse permutation (Task 1 - NASM Assembly)**

The task was about to implement function **bool inverse_permutation(size_t n, int *p)** in assembly language that is later going to be called from C level programs.

The function's arguments are a pointer **p** to a non-empty array of integers and the size of that array **n**. If the array pointed to by **p** contains a permutation of numbers between 0 and n-1, the function reverses that permutation in place and the function returns **true**. Otherwise, the result of the function is **false**, and the contents of the array pointed to by **p** after the function finishes are the same as when it was called.

## **Core (Task 2 - NASM Assembly)**

The goal of the task was to implement a distributed stack machine simulator in x86_64 assembly language. 
The machine consists of **N** cores that are numbered from 0 to N − 1, where **N** is some constant set during the compilation of simulator.

The task was about to implement function **uint64_t core(uint64_t n, char const *p)** in assembly language that is later going to be called from C level programs.

The **n** parameter contains the core number. The **p** parameter is a pointer to the ASCIIZ string and defines the calculation to be performed by the core. 
A computation consists of operations performed on the stack.

After the core has finished performing the calculation, its result, i.e. the result of the core function, is the value from the top of the stack.

