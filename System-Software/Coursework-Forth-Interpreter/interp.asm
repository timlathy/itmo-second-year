; vim: syntax=nasm

%define pc     r15 ; next instruction address
%define w      r14 ; current word address
%define rstack r13 ; call stack for nested words

section .bss
resq 1023
rstack_start: resq 1

%include "dict.inc"

native 'exit', exit, 0
  mov pc, [rstack]
  add rstack, 8
  jmp next

native '+', plus, 0
  pop rax
  add [rsp], rax
  jmp next

native '.', dot, 0
  pop rdi
  call print_int
  jmp next

native 'bye', bye, 0
  call system_exit

section .text
global _start

next:
  mov w, [pc]
  add pc, 8
  jmp [w]

docol:
  sub rstack, 8
  mov [rstack], pc
  add w, 8
  mov pc, w
  jmp next

program:
  dq xt_plus, xt_dot, xt_bye

_start:
  mov rstack, rstack_start
  mov pc, program
  push 5
  push 7
  jmp next
