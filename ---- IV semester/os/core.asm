global core
extern get_value
extern put_value

NULL_TERM 	equ 	0x0
STACK_ALIGN_VAL equ 	0x8
DIV16_CHECK 	equ 	0xF
BIT8_WORD 	equ 	0x8

section .data

align 8
is_ready: times N dq N				; Array to tell which threads want to exchange with which thread. 

align 8
values: times N dq N				; Array in which threads will exchange their values.

section .text

; N   - value defined during compilation
; rdi - uint64_t n, number of a process
; rsi - pointer to the beggining of the char const *p string
; rbp - stores the value of the stack pointer before starting the computations
; rbx - stores the address of the beggining of the values array
; r12 - stores the address of the beggining of the is_ready array

core:						; Distributed stack machine - function defined in task description.
	push 	r12				; Save r12, rbx and rbp register value, 
	lea 	r12, [rel is_ready]		; as we use these registers later, to store the address of each array
	push 	rbx				; and at the end their value has to be restored.
	lea 	rbx, [rel values]
	push 	rbp				
	mov 	rbp, rsp			; Save the value of the empty stack pointer to rbp register, as later we have to restore it.

.check_operation:				; Resolving next character from the string - if it's not '\0', perform the operation. 
	xor 	rax, rax
	mov 	al, [rsi]			; Load the next character from the instruction string to al register.
	test 	al, al				; If we have not reached the null terminator, continue computing, else just return.
	jz 	.return
	cmp 	al, '+'
	je 	.case_plus
	cmp 	al, '*'
	je 	.case_mul
	cmp 	al, '-'
	je 	.case_minus
	cmp 	al, 'n'
	je 	.case_n
	cmp 	al, 'B'
	je 	.case_B
	cmp 	al, 'C'
	je 	.case_C
	cmp 	al, 'D'
	je 	.case_D
	cmp 	al, 'E'
	je 	.case_E
	cmp 	al, 'G'
	je 	.case_G
	cmp 	al, 'P'
	je 	.case_P
	cmp 	al, 'S'
	je 	.case_S

.case_default:					; Add value from 0 to 9 to the stack, depending on the character value.
	sub 	al, '0'
	push 	rax

.case_end:					; Increment pointer to the next character in the string
	inc 	rsi				; and return back to the new instruction.
	jmp 	.check_operation

.return:					; Return the value from the top of the stack - as given in task description.
	pop 	rax				; Save the result of 'core' function - value of the top of the stack to rax register.
	leave					; Leave performs operations as: mov rsp, rbp -> pop rbp (restores empty stack and rbp value).
	pop 	rbx				; Restore the value of rbx and r12 as it has to remain unchanged after ending the program.
	pop 	r12
	ret

.case_plus:					; Pop top value of the stack and add it to current value on top of the stack.
	pop 	rcx				; In overall label adds two top values of a stack to each other and pushes 
	add 	[rsp], rcx			; addition result to the top of the stack.
	jmp 	.case_end

.case_mul:				
	pop 	rax				; Pop two top values of the stack,
	pop 	rcx
	mul 	rcx				; multiply them
	push 	rax				; and push the result of the multiplication to the top of the stack.
	jmp 	.case_end

.case_minus:					; Performs arithmetic negation of the value on top of the stack.
	neg 	qword [rsp]
	jmp 	.case_end

.case_n:					; Push n to the stack, n is saved in rdi register, given as 'core' function argument.
	push 	rdi
	jmp 	.case_end

.case_B:					; Pop value that is on top of the stack and (maybe) move index in string.
	pop 	rcx
	cmp 	qword [rsp], NULL_TERM		; If the value that is currently on top of the stack is not 0,
	jne 	.case_B_moving			; move index in the instruction string by the pop'ed value positions.
	jmp 	.case_end

.case_B_moving:
	add 	rsi, rcx			; Add pop'ed value to the address of the current position in instruction string.
	jmp 	.case_end			; Which in overall means, we are moving that much positions.

.case_C:					; Pop top value from the stack and abandon it (do nothing with it).
	pop 	rax
	jmp 	.case_end

.case_D:					; Pushes the value on top of the stack to the top of the stack. 
	push 	qword [rsp]			; In overall it means that we duplicate value on top of the stack.
	jmp 	.case_end

.case_E:					; Exchanges the two top values on top of the stack with each other.
	pop 	rcx
	xchg	rcx, [rsp]			; Swap previous top value with the value on top of the stack.
	push 	rcx 
	jmp 	.case_end

.case_G:					; Calls get_value function rdi as argument and pushes the returned value to the top of the stack.
	push 	rsi				; Save rdi and rsi registers, because get_value function may change them.
	push 	rdi		
	test 	rsp, DIV16_CHECK		; Check whether the value of the stack pointer is divisible by 16.
	jz	.call_get_value			; If it is call get_value function without aligning the stack.

.call_get_value_aligned:			; If the stack was not aligned, that it, rsp is not divisible by 16,
	sub 	rsp, STACK_ALIGN_VAL 		; align it, by subtracting 0x8 from the top of the stack pointer value
	call 	get_value			; and then call get_value function.
	add 	rsp, STACK_ALIGN_VAL		; Later we need to resolve stack value from before calling the function,
	jmp 	.exit_get_value			; so now we add 0x8 to the stack pointer value, as earlier we subtracted it.

.call_get_value: 				; If the stack was already aligned, that is, rsp already divisible by 16,
	call 	get_value 			; just call get_value function.

.exit_get_value:				; Restore previous values of rdi and rsi, because we don't know
	pop 	rdi				; how get_value function is implemented so it may change them.
	pop 	rsi
	push 	rax				; Then push the returned value of the get_value function,
	jmp 	.case_end			; that is stored in register rax to the top of the stack and continue.

.case_P:					; Takes off top value of the stack and calls put_value, with rdi and rsi as arguments.
	xchg 	rsi, qword [rsp]		; Load top stack value to rsi, as rsi is the second argument of put_value function.
	push	rdi				; Save rdi and rsi registers value, because put_value function may change them.			
	test 	rsp, DIV16_CHECK		; Check whether the value of the stack pointer is divisible by 16.
	jz	.call_put_value			; If it is call put_value function without aligning the stack.

.call_put_value_aligned: 			; If the stack was not aligned, that it, rsp is not divisible by 16,
	sub 	rsp, STACK_ALIGN_VAL		; align it, by subtracting 0x8 from the top of the stack pointer value
	call 	put_value			; and then call put_value function.
	add 	rsp, STACK_ALIGN_VAL		; Later we need to resolve stack value from before calling the function,
	jmp 	.exit_put_value			; so now we add 0x8 to the stack pointer value, as earlier we subtracted it.

.call_put_value:				; If the stack was already aligned, that is, rsp already divisible by 16,
	call 	put_value			; just call put_value function.

.exit_put_value:				; Restore previous values of rdi and rsi, because we don't know
	pop 	rdi				; how put_value function is implemented so it may change them.
	pop 	rsi
	jmp 	.case_end

.case_S:					; Synchronize threads, switch values from top of the stack between multiple threads.
	pop 	rcx				; Rcx is now the number of the thread, we want to trade with.
	pop 	qword [rbx + rdi*BIT8_WORD]	; Pop from stack, push to values[rdi], the value that we want to give to other thread.
	mov 	[r12 + rdi*BIT8_WORD], rcx	; Load to is_ready[rdi] thread number that we are waiting to trade with.
.busy_wait:
	cmp 	[r12 + rcx*BIT8_WORD], rdi	; If is_ready[rcx] is equal to rdi, that means that thread rcx now want to trade with thread rdi.
	jne 	.busy_wait			; If other thread doesn't yet want to trade, then wait for it.
	push 	qword [rbx + rcx*BIT8_WORD]	; Save the desired value, that thread rcx wanted from thread rdi on the top of the stack.
	mov 	[r12 + rcx*BIT8_WORD], rcx	; Save rcx to is_ready[rcx] to tell that the value from thread m was received by thread n.

.synchronize_threads:
	cmp 	[r12 + rdi*BIT8_WORD], rdi	; If both threads know that their values were received, then they can continue.
	jne 	.synchronize_threads
	jmp 	.case_end
