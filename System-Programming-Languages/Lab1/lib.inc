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

; ===== print_uint =====
;
; Prints an unsigned 64-bit integer.
;
; The routine first converts the number to a string,
; repeatedly dividing it by 10 (radix) and pushing the remainders on stack,
; then performs a single syscall to output the whole string.
;
; Inputs:
; * rdi — the number to be printed
print_uint:
    xor rcx, rcx        ; rcx is the number of digits in the resulting string,
                        ; used as an offset to push 8-bit characters on stack
                        ; (PUSH works with words only)
    mov rax, rdi        ; rax is the lower 64 bits of the dividend (the input number)
    mov rdi, 10         ; 10 is the radix
print_uint_convert_loop:
    xor rdx, rdx        ; rdx is the higher 64 bits of the dividend (always 0)
    div rdi             ; rax <- quotient (remaining number),
                        ; rdx <- remainder (current digit)
    add rdx, 0x30       ; the ASCII character codes for digits start with 0x30
    mov [rsp+rcx-1], dl ; move the digit (lower 8 bits) to _the bottom_ of the stack,
                        ; since diving by radix yields the digits in reverse order.
                        ; also subtract 1 to leave the space for NULL terminator
    dec rcx             ; the stack grows toward lower addresses
    cmp rax, 0          ; if the quotient is 0, we have converted the whole number
    jne print_uint_convert_loop
print_uint_output_string:
    mov byte [rsp], 0   ; terminate the string with NULL
    add rsp, rcx        ; reserve the space we've used on the stack
    mov rdi, rsp
    call print_string
    ret
