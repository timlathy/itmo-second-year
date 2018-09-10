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
