; vim: syntax=nasm

; === Comparison operations ===

; ( a b -- c ), c is the result of a comparison
%macro native_cmp_flags 3
native %1, %2, 0
  xor edx, edx ; used to temporarily store the flag
  pop rax
  cmp [rsp], rax
  %3 dl
  mov [rsp], rdx
endnative
%endmacro

native_cmp_flags '=', equals, sete   ; a == b
native_cmp_flags '<', less, setl     ; a < b
native_cmp_flags '<=', loreqs, setle ; a <= b

; === Logical operations ===

; ( 0/1 -- 1/0 )
native 'not', lnot, 0
  cmp qword [rsp], 0
  setz al
  movzx eax, al
  mov [rsp], rax
endnative

; === Bitwise operations ===

native 'and', bitand, 0
  pop rax
  and [rsp], rax
endnative

native 'or', bitor, 0
  pop rax
  or [rsp], rax
endnative

; === Arithmetic operations ===

; ( a b -- c ), c is a + b
native '+', adds, 0
  pop rax
  add [rsp], rax
endnative

; ( a b -- c ), c is a - b
native '-', subs, 0
  pop rax
  sub [rsp], rax
endnative

; ( a -- a+1 )
native '1+', incr, 0
  inc qword [rsp]
endnative

; ( a -- a-1 )
native '1-', decr, 0
  dec qword [rsp]
endnative

; ( a b -- c ), c is a * b
; mul(s) = multiplication is signed
native '*', muls, 0
  pop rax
  pop rdx
  imul rdx
  push rax
endnative

; ( a b -- c ), c is a / b
; div(s) = division is signed
native '/', divs, 0
  xor edx, edx ; upper qword of the dividend
  pop rax
  pop rcx
  idiv rcx ; rax <- quotient, rdx <- remainder
  push rax
endnative

; ( a b -- c ), c is a % b
native '%', mods, 0
  xor edx, edx ; upper qword of the dividend
  pop rax
  pop rcx
  idiv rcx ; rax <- quotient, rdx <- remainder
  push rdx
endnative
