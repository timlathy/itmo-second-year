; vim: syntax=nasm

; Pushes the next qword in the instruction stream and skips it.
native 'lit', lit, 0
  push qword [pc]
  add pc, 8
endnative

; ( ptr -- )
; Pops the top of the to HERE and advances HERE
native ',', comma, 0
  mov rax, [HERE]
  pop qword [rax]
  add qword [HERE], 8
endnative

; [ (left bracket) enters compile mode
native '[', lbrac, 1
  xor immed, immed ; immediate? = 0
endnative

; ] (right bracket) exits compile mode
native ']', rbrac, 1
  mov immed, 1 ; immediate? = 1
endnative

; Pushes interpreter state (1 = immediate mode, 0 = compile mode)
native 'immediate-mode?', immediate_mode_q, 0
  push immed
endnative

; ( word-header -- immed-flag )
native 'immediate-word?', immediate_word_q, 0
  pop rax
  mov al, byte [rax + 8] ; the next byte after the prev word ptr is (name length << 1) | flag
  and rax, 1
  push rax
endnative

; ( exec-token -- )
native 'execute', execute, 0
  pop rax
  mov w, rax
  jmp [rax]
endnative

; ( str str-len -- word-header )
native 'find', find, 0
  pop rsi ; rsi <- string length
  pop rdi ; rdi <- string
  call native_find_word ; rax <- word-header-ptr or 0 if not found
  push rax 
endnative

; ( word-header -- exec-token )
; Execution token is the address of the first command in a word
native 'cfa', cfa, 0
  pop rax
  movzx ecx, byte [rax + 8] ; first qword = word ptr, next byte = name length and flags
  shr rcx, 1 ; drop flags
  lea rax, [rax + 8 + 1 + rcx] ; skip word ptr (8), length and flags (1), name string
  push rax
endnative

; ( str str-len -- )
; Creates a new dictionary entry for a word with the specified name and immediate flag set to 0.
native 'create', create, 0
  mov rsi, [LAST_WORD] ; rsi <- last word address
  mov rax, [HERE]      ; rax <- start of the new word
  ; filling in the word header:
  mov [rax], rsi       ; the first qword in a WH is the previous word ptr
  mov [LAST_WORD], rax ; the new word is now the last defined word
  add rax, 8
  ; the next byte is (name length << 1) | immediate flag (= 0)
  pop rdx           ; string length
  lea r8, [rdx * 2] ; we'll need the length later so we use an intermediate register to shift it (* 2 = << 1)
  mov byte [rax], r8b
  ; next comes the name string
  pop rsi            ; name string buffer (copy from)
  lea rdi, [rax + 1] ; word header name string buffer (copy to)
  call native_string_copy ; thrashes rax!
  add rdi, rdx    ; rdi <- execution token address (native_string_copy preserves rdi and rdx)
  mov [HERE], rdi ; HERE now points at the first byte after the header
endnative

; compile-only

native 'branch', branch, 0
  mov pc, [pc]
endnative

native '0branch', zbranch, 0
  pop rax
  test rax, rax
  jnz zbranch_not_taken
  mov pc, [pc] ; the qword immediately following 0branch is the target address
endnative
zbranch_not_taken:
  add pc, 8 ; skip the target address qword
endnative
