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

native '.', dot, 0
  pop rdi
  call native_print_int
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
