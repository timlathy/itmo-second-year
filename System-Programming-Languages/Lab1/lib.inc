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
    cmp byte [rdi+rax], 0 ; compare each character against the NULL terminator
    je string_length_ret  ; once it's reached, return the index _before_ incrementing
                          ; the counter so the terminator isn't counted toward length
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
; * rdx — buffer size
; Outputs:
; * rax — pointer to the target buffer if the string
;         fits within the buffer, otherwise 0
string_copy:
    xor rcx, rcx ; rcx is the number of characters copied
string_copy_next_char:
    cmp rcx, rdx ; if we've reached the end of the buffer without encountering
                 ; the NULL terminator, we've hit an overflow
    je string_copy_len_overflow
    mov al, byte [rdi+rcx]
    mov byte [rsi+rcx], al
    inc rcx
    cmp al, 0    ; return successfully once the terminator is found and copied
    jne string_copy_next_char
string_copy_ret:
    mov rax, rsi
    ret
string_copy_len_overflow:
    xor rax, rax
    ret

; ===== string_equals =====
;
; Compares two strings character by character.
;
; Inputs:
; * rdi — pointer to the first string buffer
; * rsi — pointer to the second string buffer
; Outputs:
; * rax — 1 if the strings are equal, 0 otherwise
string_equals:
    xor rcx, rcx            ; rcx is the character index
string_equals_next_char:
    mov al, byte [rdi+rcx]
    cmp al, byte [rsi+rcx]
    jne string_equals_false ; if two characters are equal...
    inc rcx
    cmp al, 0               ; ...and one of them is a NULL terminator,
                            ; we consider the strings equal
    jne string_equals_next_char
string_equals_true:
    mov rax, 1
    ret
string_equals_false:
    xor rax, rax
    ret

; ===== print_string =====
;
; Prints a string using the sys_write system call.
;
; Inputs:
; * rdi — pointer to the first character of a null-terminated string
print_string:
    call string_length ; compute the length of the string
    mov rdx, rax       ; string length
    mov rsi, rdi       ; pointer to the string buffer
    mov rdi, 1         ; file descriptor (1 = stdout)
    mov rax, 1         ; syscall (1 = write)
    syscall
    ret

; ===== print_char =====
;
; Prints a single character using the sys_write system call.
;
; Inputs:
; * dil — the character to be printed
print_char:
    dec rsp        ; reserve 1 byte on the stack for the character
    mov [rsp], dil ; (PUSH only works with words)
    mov rsi, rsp   ; the character's on top of the stack now
    mov rdx, 1     ; buffer length = 1
    mov rdi, 1     ; fd (1 = stdout)
    mov rax, 1     ; syscall (1 = write)
    syscall
    inc rsp        ; free the space we've reserved on stack
    ret

; ===== print_char =====
;
; Prints a newline character (0xa) using the sys_write system call.
print_newline:
    mov dil, 0xa
    call print_char
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
    jbe read_word_loop
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

; ===== parse_uint =====
;
; Attempts to parse an uninsigned int from the given string
; up to the first non-digit character encountered.
;
; Inputs:
; * rdi — pointer to the string
;
; Outputs:
; * rax — the parsed number (0 if the string did not contain a number,
;         check rdx to verify if the string contained a number)
; * rdx — the number of characters parsed
parse_uint:
    xor rax, rax     ; rax is the number accumulated by adding and shifting digits
    xor rdx, rdx     ; rdx is the higher 64 bits of the MUL operand (needs to be 0)
    xor rcx, rcx     ; rcx is the number of digits parsed
    mov r8, 10       ; r8 is radix
    xor rsi, rsi     ; rsi is a character buffer for iterating the string
parse_uint_loop:
    mov sil, [rdi+rcx]
    cmp sil, 0x30    ; digit character codes start with 0x30
    jb parse_uint_ret
    cmp sil, 0x39    ; digit character codes end with 0x39
    ja parse_uint_ret
    sub sil, 0x30
    mul r8           ; multiply the number by radix (10) to shift the digits
    add rax, rsi     ; append the lower digit
    inc rcx
    jmp parse_uint_loop
parse_uint_ret:
    mov rdx, rcx
    ret

; ===== parse_int =====
;
; Attempts to parse a nsigned int from the given string
; up to the first non-digit character encountered.
;
; Inputs:
; * rdi — pointer to the string
;
; Outputs:
; * rax — the parsed number (0 if the string did not contain a number,
;         check rdx to verify if the string contained a number)
; * rdx — the number of characters parsed
parse_int:
    cmp byte [rdi], '-'
    je parse_int_negative
    call parse_uint ; if the string does not contain a '-' sign,
                    ; we just treat the number as unsigned
    ret
parse_int_negative:
    inc rdi         ; otherwise, we need to skip the '-' sign we've read
    call parse_uint
    test rdx, rdx   ; and if the number of digits parsed is more than 0
    je parse_int_ret
    neg rax         ; we should negate the result
    inc rdx         ; and add up the '-' sign to the parsed character count
parse_int_ret:
    ret
