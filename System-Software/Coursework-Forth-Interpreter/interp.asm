; vim: syntax=nasm

%define pc     r15 ; next instruction address
%define w      r14 ; current word address
%define rstack r13 ; call stack for nested words

section .bss
resq 1023
rstack_start: resq 1

%include "native-words.inc"

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
  dq xt_drop, xt_plus, xt_dot, xt_bye

_start:
  mov rstack, rstack_start
  mov pc, program
  push 5
  push 7
  push 9
  jmp next
