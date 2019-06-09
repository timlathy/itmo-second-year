; vim: syntax=nasm

; === native_find_word ===
; Looks up a word in the dictionary.
;
; * rdi — (in) string pointer
; * rsi — (in) string length
; * rax — (out) word header address (0 if not found)
native_find_word:
  mov rax, [LAST_WORD]
find_word_loop:
  test rax, rax             ; rax is the pointer to a word header
  jz find_word_return       ; rax == 0 => we've traversed the whole dictionary
                            ; but did not find the word we're looking for
  movzx ecx, byte [rax + 8] ; the first 8 bytes of WH are occupied by the next word
                            ; pointer, the byte after it is the word name length
                            ; (shifted by 1) and immediate flag in the lower bit
  shr rcx, 1                ; remove the flag to get word name length
  cmp rcx, rsi              ; are name lengths equal?
  je find_word_loop_cmp
find_word_loop_next:
  mov rax, [rax]
  jmp find_word_loop
find_word_loop_cmp:
  xor ecx, ecx
find_word_loop_cmp_loop:
  mov dl, [rdi + rcx]
  cmp byte [rax + 8 + 1 + rcx], dl ; base + next ptr (8) + name length (1) = name start
  jne find_word_loop_next
  inc rcx
  cmp rcx, rsi
  jne find_word_loop_cmp_loop
find_word_return:
  ret ; if we arrive here from find_word_loop_cmp_loop,
      ; rax contains the word header address
