; vim: syntax=asm

section .text

; ===== string_length =====
;
; Computes the length of a string.
;
; Inputs:
; * rdi — pointer to the first character of a null-terminated string
; Outputs:
; * rax — the length of the string
string_length:
    xor rax, rax
string_length_loop:
    cmp byte [rdi+rax], 0
    je string_length_ret
    inc rax
    jmp string_length_loop
string_length_ret:
    ret

; ===== string_copy =====
;
; Copies a string from the source to the target buffer.
;
; Inputs:
; * rdi — pointer to the source buffer
; * rsi — pointer to the target buffer
; * rdx — length of the string
string_copy:
    xor rcx, rcx
string_copy_loop:
    cmp rcx, rdx
    je string_copy_ret
    mov al, byte [rdi+rcx]
    mov byte [rsi+rcx], al
    inc rcx
    jmp string_copy_loop
string_copy_ret:
    ret

; ===== print_string =====
;
; Prints a string using the sys_write system call.
;
; Inputs:
; * rdi — pointer to the first character of a null-terminated string
print_string:
    push rsi     ; rsi is a callee-saved register that we use later on
    call string_length
    mov rdx, rax ; rdx specifies the number of characters in a string
    mov rsi, rdi ; rsi points to the buffer sys_write reads from
    mov rdi, 1   ; 1 = stdout
    mov rax, 1   ; syscall #1 is sys_write
    syscall
    mov rdi, rsi ; we've moved rdi to rsi, now we must restore it
    pop rsi      ; restore rsi
    ret

; ===== print_char =====
;
; Prints a single character using the sys_write system call.
;
; Inputs:
; * rdi — the character to be printed
print_char:
    push rdi
    mov rsi, rsp
    mov rdx, 1   ; buffer length = 1
    mov rdi, 1   ; print to stdout
    mov rax, 1   ; sys_write
    syscall
    add rsp, 8
    ret

; ===== convert_uint_to_string =====
;
; Converts an unsigned 64-bit integer to string,
; repeatedly dividing it by 10 (radix) and
; pushing the ASCII codes of remainders on stack.
;
; Inputs:
; * rdi — the number to be converted
; Outputs:
; The string (NULL-terminated) is placed on stack.
convert_uint_to_string:
    pop rcx           ; store the return address
    mov rax, rsp
    dec rsp           ; first, we push the NULL terminator to the bottom of the stack,
    mov byte [rsp], 0 ;   where the string will end (doing it manually because PUSH
                      ;   only works with words)
    mov rax, rdi      ; rax is the lower 64 bits of the dividend (the input number)
    mov rdi, 10       ; 10 is the radix
convert_uint_to_string_loop:
    xor rdx, rdx      ; rdx is the higher 64 bits of the dividend (always 0)
    div rdi           ; rax <- quotient (remaining number),
                      ; rdx <- remainder (current digit)
    add rdx, 0x30     ; ASCII character codes for digits start with 0x30
    dec rsp
    mov [rsp], dl     ; move the digit (lower 8 bits) to _the bottom_ of the stack,
                      ; since diving by radix yields the digits in reverse order.
    cmp rax, 0        ; if the quotient is 0, we have converted the whole number
    jne convert_uint_to_string_loop
    push rcx          ; restore the return address
    ret

; ===== print_uint =====
;
; Prints an unsigned 64-bit integer using a single sys_write call.
;
; Inputs:
; * rdi — the number to be converted
print_uint:
    push rbx
    mov rbx, rsp                ; we save rsp to rbx to clear the stack later
    call convert_uint_to_string ; the string is at the top of the stack now
    mov rdi, rsp                ; print_string expects the string pointer in rdi
    call print_string
    mov rsp, rbx                ; clear the stack
    pop rbx                     ; restore rbx (callee-saved)
    ret

; ===== print_int =====
;
; Prints a signed 64-bit integer.
;
; The routine is similar to print_uint, except that
; it additionally checks if the number is negative
; and prints a leading '-' if it is.
;
; Inputs:
; * rdi — the number to be printed
print_int:
    push rbx
    mov rbx, rsp
    test rdi, rdi
    jns print_int_positive
print_int_negative:
    neg rdi
    call convert_uint_to_string
    dec rsp
    mov byte [rsp], '-'
    jmp print_int_epilogue
print_int_positive:
    call convert_uint_to_string
print_int_epilogue:
    mov rdi, rsp
    call print_string
    mov rsp, rbx
    pop rbx
    ret

; ===== read_char =====
;
; Reads a single character from stdin into al.
;
; Outputs:
; * al — the character read from stdin,
;        or 0x0 if stdin has been closed/an error has occurred
read_char:
    xor rax, rax ; sys_read is system call #0
    xor rdi, rdi ; fd = 0 (stdin)
    mov rdx, 1   ; read 1 char
    dec rsp      ; reserve one byte on the stack, which we'll use a buffer 
    mov rsi, rsp ; rsp now points to our single-character buffer
    syscall
    cmp rax, 1   ; sys_read returns the number of bytes read;
                 ; if it's 0, the stdin has been closed,
                 ; if it's -1, an error has occurred (see man 2 read)
    jne read_char_error
read_char_success:
    mov al, [rsp]
    inc rsp
    ret
read_char_error:
    xor rax, rax
    inc rsp
    ret

; ===== read_word =====
;
; Reads a word from stdin character by character,
; skipping leading whitespace. Accepts a word size limit;
; if the word read from stdin exceeds it, returns 0,
; otherwise returns the word buffer pointer.
;
; Inputs:
; * rdi — pointer to the word buffer
; * rsi — maximum word size
;
; Outputs:
; * rax — the word buffer pointer if the word fits
;         within the character limit, otherwise 0
; * rdx — the number of characters in a word (0 if the limit is exceeded)
read_word:
    push r12
    push r13
    push r14
    mov r12, rsi            ; r12 is the maximum word size
    mov r13, rdi            ; r13 is the word buffer pointer
    xor r14, r14            ; r14 is the word length counter
read_word_skip_spaces:
    call read_char
    cmp al, ' '
    je read_word_skip_spaces
    jmp read_word_write_to_buf
read_word_loop:
    call read_char
    cmp al, ' '
    je read_word_success
read_word_write_to_buf:
    cmp al, 0x0             ; stdin closed
    je read_word_success
    cmp al, 0x9             ; tab
    je read_word_success
    mov [r13+r14], al
    inc r14
    cmp r14, r12
    jle read_word_loop
read_word_count_exceeded:
    xor rax, rax
    xor rdx, rdx
    jmp read_word_ret
read_word_success:
    mov byte [r13+r14], 0x0 ; null-terminate the string
    mov rax, r13
    mov rdx, r14
read_word_ret:
    pop r14
    pop r13
    pop r12
    ret
