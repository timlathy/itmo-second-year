; vim: syntax=nasm
; Copied (with minor modifications) from System-Programming-Languages/Lab1

; As in System V AMD64 ABI, only rbx, rbp, and r12-r15 are preserved,
; the rest are volatile (caller-saved).

; === native_exit ===
; Terminates the program with an exit code of 0 .
native_exit:
  mov rax, 60        ; syscall (60 = exit)
  mov rdi, 0         ; return code
  syscall

; === native_print_string ===
; Prints a string using the sys_write system call.
;
; * rsi — (in) string pointer
; * rdx — (in) string length
native_print_string:
  mov rax, 1 ; syscall (1 = write)
  mov rdi, 1 ; fd (1 = stdout)
  syscall    ; rsi (string pointer) and rdx (string length) are already filled in
  ret

; === native_print_int ===
; Prints a signed 64-bit integer.
;
; * rdi — (in) the number to be printed
native_print_int:
  push rbx     ; callee-saved
  mov rbx, rsp ; rbx <- initial stack position
  test rdi, rdi
  jns print_int_positive
print_int_negative:
  neg rdi
  call native_uint_to_string ; rax <- string length, stack <- string
  dec rsp
  mov byte [rsp], '-'
  jmp print_int_epilogue
print_int_positive:
  call native_uint_to_string ; rax <- string length, stack <- string
print_int_epilogue:
  mov rsi, rsp ; rsi <- string pointer
  mov rdx, rax ; rdx <- string length
  call native_print_string
  mov rsp, rbx ; restore the initial stack position, clearing the string off of it
  pop rbx
  ret

; === native_uint_to_string ===
; Converts an unsigned 64-bit integer to string, repeatedly dividing it by 10 (radix)
; and pushing the ASCII codes of remainders on stack.
;
; * rdi (in) — the number to be printed
; * rax (out) — string length
; * stack (out) — the resulting string
native_uint_to_string:
  pop rsi           ; rsi <- return address (the stack'll contain the resuling string)
  xor rdx, rdx      ; rcx <- string length (0 initially)
  mov rax, rdi      ; rax is the lower 64 bits of the dividend (the input number)
  mov rdi, 10       ; 10 is the radix
uint_to_string_loop:
  inc rcx           ; string length += 1
  xor rdx, rdx      ; rdx is the higher 64 bits of the dividend (always 0)
  div rdi           ; rax <- quotient (remaining number),
                    ; rdx <- remainder (current digit)
  add rdx, 0x30     ; ASCII character codes for digits start with 0x30
  dec rsp
  mov [rsp], dl     ; move the digit (lower 8 bits) to _the bottom_ of the stack,
                    ; since diving by radix yields the digits in reverse order.
  cmp rax, 0        ; if the quotient is 0, we have converted the whole number
  jne uint_to_string_loop
  mov rax, rcx      ; rax <- resulting string length
  push rsi          ; restore return address
  ret

; === native_read_word ===
; Reads a word from stdin character by character, skipping leading whitespace.
; Accepts a word size limit; if the word read from stdin exceeds it, returns 0,
; otherwise returns the word buffer pointer.
;
; * rdi — (in) pointer to the word buffer
; * rsi — (in) maximum word size
; * rax — (out) the word buffer pointer if the word fits
;         within the character limit, 0 otherwise
; * rdx — (out) the number of characters in a word (0 if the limit is exceeded)
native_read_word:
  push r12
  push r13
  push r14
  mov r12, rsi            ; r12 is the maximum word size
  mov r13, rdi            ; r13 is the word buffer pointer
  xor r14, r14            ; r14 is the word length counter
read_word_skip_spaces:
  call native_read_char
  cmp al, ' '
  je read_word_skip_spaces
  jmp read_word_write_to_buf
read_word_loop:
  call native_read_char
  cmp al, ' '
  je read_word_success
read_word_write_to_buf:
  cmp al, 0x0             ; stdin closed
  je read_word_success
  cmp al, 0x9             ; tab
  je read_word_success
  cmp al, 0xA             ; newline
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
  mov rax, r13
  mov rdx, r14
read_word_ret:
  pop r14
  pop r13
  pop r12
  ret

; === native_read_char ===
; Reads a single character from stdin into al.
;
; * al — (out) the character read from stdin,
;        or 0x0 if stdin has been closed/an error has occurred
native_read_char:
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
