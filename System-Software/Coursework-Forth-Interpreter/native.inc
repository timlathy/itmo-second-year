; vim: syntax=nasm

native 'lit', lit, 0
  push qword [pc]
  add pc, 8
endnative

; stack operations

; ( a b -- a )
native 'drop', drop, 0
  pop rax
endnative

; ( a b -- b )
native 'nip', nip, 0
  pop rax
  mov [rsp], rax
endnative

native 'swap', swap, 0
  pop rax ; stack top
  pop rbx ; stack top - 1
  push rax
  push rbx ; new stack top
endnative

native 'dup', dup, 0
  push qword [rsp]
endnative

; ( a b -- a b a b )
native '2dup', twodup, 0
  mov rdx, [rsp + 8] ; a
  mov rax, [rsp]     ; b
  push rdx           ; a b a
  push rax           ; a b a b
endnative

; math

native '+', plus, 0
  pop rax
  add [rsp], rax
endnative

native '-', minus, 0
  pop rax
  sub [rsp], rax
endnative

; strings

; ( str str-len -- num parsed-str-len )
native 'number', number, 0
  pop rsi ; rsi <- string length
  pop rdi ; rdi <- string
  call parse_int
  push rax ; number
  push rdx ; parsed-str-len
endnative

; interpreter

; ( execution-token -- )
native 'execute', execute, 0
  pop rax
  mov w, rax
  jmp [rax]
endnative

; ( str str-len -- word-header-ptr )
native 'find', find, 0
  pop rsi ; rsi <- string length
  pop rdi ; rdi <- string
  call native_find_word ; rax <- word-header-ptr or 0 if not found
  push rax 
endnative

; ( word-header-ptr -- execution-token-ptr )
; Execution token is the address of the first command in a word
native 'cfa', cfa, 0
  pop rax
  movzx rcx, byte [rax + 8] ; first qword = word ptr, next byte = name length and flags
  shr rcx, 1 ; drop flags
  lea rax, [rax + 8 + 1 + rcx] ; skip word ptr (8), length and flags (1), name string
  push rax
endnative

; ( str str-len -- )
; Creates a new dictionary entry for a word with the specified name and immediate flag set to 0.
native 'create', create, 0
  mov rsi, [dict_last_word] ; rsi <- last word address
  mov rax, [HERE]           ; rax <- start of the new word
  ; filling in the word header:
  mov [rax], rsi            ; the first qword in a WH is the previous word ptr
  mov [dict_last_word], rax ; the new word is now the last defined word
  add rax, 8
  ; the next byte is (name length << 1) | immediate flag (= 0)
  pop rdx           ; string length
  lea r8, [rdx * 2] ; we'll need the length later so we use an intermediate register to shift it (* 2 = << 1)
  mov [rax], r8
  ; next comes the name string
  pop rsi            ; name string buffer (copy from)
  lea rdi, [rax + 1] ; word header name string buffer (copy to)
  call native_copy_string
  lea rax, [rax + 1 + rdx] ; compute execution token address (native_copy_string preserves rdx)
  mov [HERE], rax          ; HERE now points at the first byte after the header
endnative

; ( ptr -- )
; Stores the top of the stack at HERE and advances HERE
native ',', comma, 0
  mov rax, [HERE]
  pop qword [rax]
  add qword [HERE], 8
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

; system

; ( num -- )
native '.', dot, 0
  pop rdi
  call native_print_int
  call native_print_newline
endnative

native '.S', dotstack, 0
  push rbx
  mov rbx, rsp
dotstack_loop:
  cmp rbx, [stack_start_ptr]
  je dotstack_ret
  mov rdi, [rbx]
  add rbx, 8
  call native_print_int
  call native_print_newline
  jmp dotstack_loop
dotstack_ret:
  pop rbx
endnative

native 'word', word, 0
  mov rdi, input_scratch
  mov rsi, max_word_length
  call native_read_word
  push rax ; str
  push rdx ; str-len
endnative

native 'exit', exit, 0
  mov pc, [rstack]
  add rstack, 8
endnative

native 'bye', bye, 0
  call native_exit
