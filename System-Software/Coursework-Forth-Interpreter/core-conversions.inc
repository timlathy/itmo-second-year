; vim: syntax=nasm

; === native_uint_to_string ===
; Converts an unsigned 64-bit integer to string at the specified location.
;
; * rdi — (in) the number to be printed
; * rsi - (in) target string pointer
; * rax — (out) string length
native_uint_to_string:
  push rbx
  mov rbx, rsp ; rbx <- old stack pointer (we use the stack as a scratch space)
  xor rdx, rdx ; rdx <- higher 64 bits of the dividend
  mov rax, rdi ; rax <- lower 64 bits of the dividend (input number)
  mov r8, 10   ; r8 <- radix
uint_to_string_loop:
  xor rdx, rdx        ; rdx is the higher 64 bits of the dividend (always 0)
  div r8              ; rax <- quotient (remaining number),
                      ; rdx <- remainder (current digit)
  add rdx, 0x30       ; ASCII character codes for digits start with 0x30
  dec rsp
  mov [rsp], dl       ; move the digit (lower 8 bits) to _the bottom_ of the stack,
                      ; since diving by radix yields the digits in reverse order.
  test rax, rax       ; if the quotient is 0, we have converted the whole number
  jnz uint_to_string_loop
uint_to_string_copy_stack: ; at this point, rax is 0
                           ; we'll use it as the resulting string length counter
uint_to_string_copy_stack_loop: ; copy digits in reverse order (pop them off the stack)
  mov dl, [rsp]
  inc rsp
  mov [rsi + rax], dl
  inc rax
  cmp rsp, rbx ; when all digits are popped, rsp matches the value stored at the beginning
  jne uint_to_string_copy_stack_loop
uint_to_string_ret:
  pop rbx
  ret

; ===== parse_uint =====
; Attempts to parse an uninsigned int from the given string
; up to the first non-digit character encountered.
;
; * rdi — (in) string pointer
; * rsi — (in) string length
; * rax — (out) parsed number
; * rdx — (out) number of characters parsed (0 if the number could not be parsed)
parse_uint:
  push rbx
  mov rbx, rsi     ; rbx <- string length
  xor rax, rax     ; rax is the number accumulated by adding and shifting digits
  xor rdx, rdx     ; rdx is the higher 64 bits of the MUL operand (needs to be 0)
  xor rcx, rcx     ; rcx is the number of digits parsed
  mov r8, 10       ; r8 is radix
  xor rsi, rsi     ; rsi is a character buffer for iterating the string
parse_uint_loop:
  cmp rcx, rbx     ; number of digits parsed = string length?
  je parse_uint_ret
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
  pop rbx
  ret

; ===== parse_int =====
; Attempts to parse a nsigned int from the given string
; up to the first non-digit character encountered.
;
; * rdi — (in) string pointer
; * rsi — (in) string length
; * rax — (out) parsed number
; * rdx — (out) number of characters parsed (0 if the number could not be parsed)
parse_int:
  test rsi, rsi     ; string length = 0?
  jmp parse_int_ret
  cmp byte [rdi], '-'
  je parse_int_negative
  call parse_uint ; if the string does not contain a '-' sign,
                  ; we treat the number as unsigned
  ret
parse_int_negative:
  inc rdi         ; otherwise, we need to skip the '-' sign we've read
  dec rsi
  call parse_uint
  test rdx, rdx   ; and if the number of digits parsed is more than 0
  je parse_int_ret
  neg rax         ; we should negate the result
  inc rdx         ; and add up the '-' sign to the parsed character count
parse_int_ret:
  ret
