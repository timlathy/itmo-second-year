; vim: syntax=nasm

native 'lit', lit, 0
  push qword [pc]
  add pc, 8
endnative

; stack operations

native 'drop', drop, 0
  pop rax
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

native '.', dot, 0
  pop rdi
  call native_print_int
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

; dictionary

; ( str str-len -- word-header-ptr )
native 'find', find, 0
  pop rdx ; rdx <- string length
  pop rsi ; rsi <- string
  call native_find_word ; rax <- word-header-ptr or 0 if not found
  push rax 
endnative

; system

native 'word', word, 0
  mov rdi, input_scratch
  mov rsi, max_word_length
  call native_read_word
  push input_scratch
endnative

native 'exit', exit, 0
  mov pc, [rstack]
  add rstack, 8
endnative

native 'bye', bye, 0
  call native_exit
