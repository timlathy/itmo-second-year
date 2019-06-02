; vim: syntax=nasm

%define pc     r15 ; next instruction address
%define w      r14 ; current word address
%define rstack r13 ; call stack for nested words

%include "dict.inc"

; stack operations

native 'drop', drop, 0
  pop rax
  jmp next

native 'swap', swap, 0
  pop rax ; stack top
  pop rbx ; stack top - 1
  push rax
  push rbx ; new stack top
  jmp next

native 'dup', dup, 0
  push qword [rsp]
  jmp next

; math

native '+', plus, 0
  pop rax
  add [rsp], rax
  jmp next

; misc

native 'exit', exit, 0
  mov pc, [rstack]
  add rstack, 8
  jmp next

native '.', dot, 0
  pop rdi
  call print_int
  jmp next

native 'bye', bye, 0
  call system_exit
