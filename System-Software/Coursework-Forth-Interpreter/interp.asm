; vim: syntax=nasm

%define pc     r15 ; next instruction address
%define w      r14 ; current word address
%define rstack r13 ; call stack for nested words

%include 'dict.inc'

section .bss

resq 1023
rstack_head: resq 1
stack_start_ptr: resq 1
input_scratch: resb 256
output_scratch: resb 256

runtime_dict: resq 32768

section .text

%include 'core-conversions.inc'
%include 'core-strings.inc'
%include 'core-io.inc'
%include 'core-dict.inc'
%include 'native.inc'
%include 'words.inc'

section .data

dict_last_word: dq __dict_last_word__
HERE: dq runtime_dict

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
  dq xt_lit, 5, xt_lit, 7
  dq xt_interpreter_loop

_start:
  mov [stack_start_ptr], rsp
  mov rstack, rstack_head
  mov pc, program
  jmp next
