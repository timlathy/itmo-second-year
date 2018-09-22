; vim: syntax=nasm

global find_word
extern string_equals

; ===== find_word =====
;
; Dictionary is a linked-list with each item consisting of an 8-byte pointer
; to the previous word (the _tail_ pointer), a null-terminated word name string,
; and an arbitrary sized word definition blob.
;
; This routine finds a word given a dictionary head pointer and a
; null-terminated name to be found.
;
; Inputs:
; * rdi — pointer to the first character of a null-terminated word name string
; * rsi — pointer to the head of the dictionary
; Outputs:
; * rax — pointer to a word if found, 0 otherwise
section .text
find_word:
  push r12
  push r13
  mov r12, rdi
  mov r13, rsi
  xor rax, rax           ; if rsi is NULL, we need to return 0 as well
find_word_loop:
  test r13, r13          ; r13 a pointer to the current word; if it's NULL,
                         ; we've reached the end of the dictionary without
                         ; finding the word we're looking for
  jz find_word_not_found
  mov rdi, r12           ; we'll call string_equals to check if the current
                         ; word's name matches what we're looking for (rdi)
  lea rsi, [r13 + 8]     ; the first 8 bytes of the word are occupied
                         ; by the tail pointer; skip them
  call string_equals
  test rax, rax          ; if the name matches, rax will contain 1
  jnz find_word_found
  mov r13, [r13]         ; otherwise, we advance to the next word by following
                         ; the tail pointer
  jmp find_word_loop
find_word_found:
  mov rax, r13
find_word_not_found:     ; string_equals sets rax to 0 if names aren't equal
  pop r13
  pop r12
  ret
