; vim: syntax=nasm

%include "dict.inc"

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
  call print_int
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

; system

native 'word', word, 0
  mov rdi, input_scratch
  mov rsi, max_word_length
  call read_word
  push input_scratch
endnative

native 'exit', exit, 0
  mov pc, [rstack]
  add rstack, 8
endnative

native 'bye', bye, 0
  call system_exit
