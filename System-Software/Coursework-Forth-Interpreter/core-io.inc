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
; * rdi — (in) string pointer
; * rsi — (in) string length
native_print_string:
  xchg rdi, rsi ; rsi <- string pointer
  mov rdx, rdi  ; rdx <- string length
  mov rax, 1    ; syscall (1 = write)
  mov rdi, 1    ; fd (1 = stdout)
  syscall
  ret

; ===== native_print_char =====
; Prints a single character using the sys_write system call.
;
; * dil — (in) the character to be printed
native_print_char:
  mov byte [output_scratch], dil
  mov rsi, output_scratch ; rsi <- string pointer
  mov rdx, 1 ; rdx <- string length
  mov rax, 1 ; syscall (1 = write)
  mov rdi, 1 ; fd (1 = stdout)
  syscall
  ret

; ===== native_print_newline =====
; Prints a newline using the sys_write system call.
native_print_newline:
  mov dil, 0xa ; '\n'
  call native_print_char
  ret

; === native_print_int ===
; Prints a signed 64-bit integer.
;
; * rdi — (in) the number to be printed
native_print_int:
  test rdi, rdi
  jns native_print_int_positive
native_print_int_negative:
  mov byte [output_scratch], '-'
  lea rsi, [output_scratch + 1]
  neg rdi
  call native_uint_to_string ; rax <- num string length
  inc rax ; rax <- string length including '-'
  jmp native_print_int_epilogue
native_print_int_positive:
  mov rsi, output_scratch
  call native_uint_to_string ; rax <- string length
native_print_int_epilogue:
  mov rdi, output_scratch ; rdi <- string pointer (note that native_uint_to_string thrashes rsi)
  mov rsi, rax ; rsi <- string length
  call native_print_string
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
