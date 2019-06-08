; vim: syntax=nasm

; === Arithmetic operators ===

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
  xor rdx, rdx ; upper qword of the dividend
  pop rax
  pop r8
  idiv r8 ; rax <- quotient, rdx <- remainder
  push rax
endnative

; ( a b -- c ), c is a % b
native '%', mods, 0
  xor rdx, rdx ; upper qword of the dividend
  pop rax
  pop r8
  idiv r8 ; rax <- quotient, rdx <- remainder
  push rdx
endnative
