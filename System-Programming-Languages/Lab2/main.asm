; vim: syntax=nasm

global _start
extern find_word
extern string_length
extern read_word
extern print_string
extern print_newline
extern panic
extern exit

section .data
msg_word_not_found: db "Word not found", 10, 0
msg_input_too_long: db "Exceeded maximum word name length (up to 255 characters allowed)", 10, 0

%include "colon.inc"

section .text
_start:
  sub rsp, 256             ; allocate 256 bytes on the stack
                           ; to store the user-supplied word name
  mov rsi, 255             ; read up to 255 characters
                           ; (one byte is reserved for the NULL terminator)
  mov rdi, rsp
  call read_word           
  test rax, rax            ; read_word returns 0 if the input exceeds the buffer,
  jz main_input_too_long  
  mov rdi, rsp
  mov rsi, colon_head      ; colon_head is the dictionary head pointer, see "colon.inc"
  call find_word
  test rax, rax            ; find_word returns 0 if the word was not found
  jz main_word_not_found  
  lea rdi, [rax + 8]       ; if the word _is_ found, we want to print its contents.
                           ; find_word returns a pointer to the word header, so we
                           ; need to skip the tail pointer (first 8 bytes) and
                           ; the name string (indeterminate length, null-terminated).
  call string_length
  lea rdi, [rdi + rax + 1] ; word contents = word name pointer + name length + null terminator
  call print_string
  call print_newline
  call exit
main_input_too_long:
  mov rdi, msg_input_too_long
  call panic
main_word_not_found:
  mov rdi, msg_word_not_found
  call panic
