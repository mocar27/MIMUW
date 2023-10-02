global inverse_permutation

ONE     equ     0x1
INTMAX  equ     0x7fffffff	
INTSIZE equ     0x4
MSBIT   equ     31

section .text

; In register rdi is the size of the array - n given as the function argument.
; In register rsi is the pointer to the array - p given as the function argument.

inverse_permutation:
        test    rdi, rdi                         	; Check whether n is less or equal to 0.
        jle     .exit_false                      	; If n <= 0, exit and return false.
	dec 	rdi					; From this point we will operate on rdi as n-1, it means we will iterate in loop until i is less or equal to n-1.
        cmp     rdi, INTMAX                       	; Check whether n-1 is greater than INT_MAX.
        jg      .exit_false				; If n-1 > INT_MAX, exit and return false.
        xor     ecx, ecx                          	; Set ecx to 0 (i = 0).

.validate_loop:
        cmp     ecx, edi                          	; Check whether i < n.
        jg      .permutation_loop                	; If i > n-1, exit loop and continue (all array elements have correct values).
        mov	edx, dword [rsi + INTSIZE * rcx]  	; Load i-th array element to register edx, eg. edx = p[i].
        test    edx, edx                         	; Check whether p[i] < 0.
        jl      .exit_false                     
        cmp     edx, edi                          	; Check whether p[i] > n-1.
        jg      .exit_false 
        inc     ecx                    			; i++.
        jmp     .validate_loop

.exit_false:
        xor     al, al                          	; Set al to 0 and return - false.
        ret

.permutation_loop:
	dec 	ecx
	test  	ecx, ecx
	jl 	.array_rearrangement_loop		; If i < 0, exit loop and continue code (array is a correct permutation).
	mov 	edx, dword [rsi + INTSIZE * rcx] 	; Load i-th array element to register edx, eg. edx = p[i].
	btr 	edx, MSBIT				; Set the 32st bit of eax to 0 (to retrieve value, without modifying the array, we want to check whether it has not yet been visited).
	mov 	eax, dword [rsi + INTSIZE * rdx]	; Load p[p[i]] to register eax, eg. eax = p[p[i]].
	test   	eax, eax				; If the value in p[p[i]] is negative, it means that this position is already taken and the permutation is wrong.
	jl 	.begin_reset_and_exit 			; If the value we want to go was already visited, there are duplicates in the array, so exit and return false.
	bts 	dword [rsi + INTSIZE * rdx], MSBIT	; Set the 32st bit of p[p[i]] to 1 (it means that current value is now visited and cannot be taken by any other value).
	jmp 	.permutation_loop

.begin_reset_and_exit:
	xor	ecx, ecx				; Reset i to 0 and continue to label resetting the array.
	jmp 	.reset_and_exit_loop

.reset_and_exit_loop:					; Retrieve previous values in the array and exit false (some values were duplicated).
	cmp 	ecx, edi
	jg 	.exit_false
	btr 	dword [rsi + INTSIZE * rcx], MSBIT	; Set the 32st bit of p[i] to 0.
	inc 	ecx
	jmp 	.reset_and_exit_loop

.array_rearrangement_loop:
	inc 	ecx 
	cmp 	ecx, edi
	jg 	.exit_true				; If i > n-1, exit and return true	
	mov 	edx, dword [rsi + INTSIZE * rcx] 	; Load i-th array element to register edx, eg. edx = p[i].
	mov 	eax, ecx				; In eax we will hold the value we want to place next.
	test 	edx, edx 				; Check whether edx < 0 (value in p[i] before flipping MSB).
	jl 	.switch_values_in_cycle 		; If edx < 0, we have not yet visited this value in cycle so we can jump to inner loop and switch values in cycle.
	jmp 	.array_rearrangement_loop

.back_on_cycle_beginning:
	mov 	dword [rsi + INTSIZE * rdx], eax 	; Place value we wanted to placed next in the correct place.
	jmp 	.array_rearrangement_loop

.switch_values_in_cycle:
	btr 	edx, MSBIT				; Set the 32st bit of edx to 0 to retrieve previous value (while loading value to edx from array it was negative).
	cmp	edx, ecx 				; Check whether edx == i (we are back on starting point of the cycle).
	je 	.back_on_cycle_beginning 		
	mov 	r8d, dword [rsi + INTSIZE * rdx] 	; Temporarly load p[p[i]] to register r8d, eg. r8d = p[p[i]].
	mov 	dword [rsi + INTSIZE * rdx], eax 	; Place value we wanted to placed next in the correct place.
	mov 	eax, edx 				; In eax we will hold the value we want to place next.
	mov 	edx, r8d 				; Now we can load value from r8d to register edx, it is our next value we want to jump to check whether it has not yet been visited.
	jmp 	.switch_values_in_cycle

.exit_true:
	mov 	al, ONE 				; Set al to 1 and return - true.
	ret
