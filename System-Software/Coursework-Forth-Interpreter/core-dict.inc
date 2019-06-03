; vim: syntax=nasm

; === native_find_word ===
; Looks up a word in the dictionary.
;
; * rsi — (in) string pointer
; * rdx — (in) string length
; * rax — (out) word header address (0 if not found)
native_find_word:
  mov rax, [dict_last_word]
find_word_loop:
  test rax, rax          ; rax is the pointer to a word header
  jz find_word_return    ; rax == 0 => we've traversed the whole dictionary
                         ; but did not find the word we're looking for
  mov cl, [rax + 8]      ; the first 8 bytes of WH are occupied by the next word
                         ; pointer, the byte after it is the word name length
  cmp rcx, rdx           ; are name lengths equal?
  je find_word_loop_cmp_loop
find_word_loop_next:
  mov rax, [rax]
  jmp find_loop  
find_word_loop_cmp_loop:
  mov rdi, [rsi + rcx]
  cmp [rax + 8 + 1 + rcx], rdi ; base + next ptr (8) + name length (1) = name start
  jne find_word_loop_next
  inc rcx
  cmp rcx, rdx
  jne find_word_loop_cmp_loop
find_word_return:
  ret ; if we arrive here from find_Word_loop_cmp_loop,
      ; rax contains the word header address
