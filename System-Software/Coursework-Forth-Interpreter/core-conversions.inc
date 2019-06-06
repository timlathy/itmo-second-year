; vim: syntax=nasm

; === native_uint_to_string ===
; Converts an unsigned 64-bit integer to string, repeatedly dividing it by 10 (radix)
; and pushing the ASCII codes of remainders on stack.
;
; * rdi — (in) the number to be printed
; * rax — (out) string length
; * stack — (out) the resulting string
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
