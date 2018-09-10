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
