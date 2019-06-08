; vim: syntax=nasm

; ===== native_string_copy =====
; Copies a string from the source to the target buffer.
;
; * rsi — (in) source buffer ptr
; * rdi — (in) target buffer ptr
; * rdx — string length
; 
; rdx, rsi, rdi are guaranteed to be preserved.
native_string_copy:
  xor rcx, rcx ; rcx <- iterator
native_string_copy_loop:
  cmp rcx, rdx
  je native_string_copy_ret
  mov al, byte [rsi + rcx]
  mov byte [rdi + rcx], al
  inc rcx
  jmp native_string_copy_loop
native_string_copy_ret:
  ret
