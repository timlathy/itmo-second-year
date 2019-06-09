; vim: syntax=nasm

; === Return stack ===

; ( a -- )
; Pushes the value off the data stack onto the return stack.
native '>r', rstackpush, 0
  pop rax
  sub rstack, 8
  mov [rstack], rax
endnative

; ( -- a )
; Pops the value off the return stack onto the data stack.
native 'r>', rstackpop, 0
  mov rax, [rstack]
  add rstack, 8
  push rax
endnative

; ( -- a )
; Copies the top of the return stack onto the data stack.
native 'r@', rstackcopy, 0
  push qword [rstack]
endnative

; === Data stack ===

; ( a b -- a )
native 'drop', drop, 0
  pop rax ; pop rax is one byte, add rsp, 8 is four
endnative

native '2drop', twodrop, 0
  pop rax ; still smaller than add rsp, 16
  pop rax
endnative

; ( a b -- b )
native 'nip', nip, 0
  pop rax
  mov [rsp], rax
endnative

; ( a b -- b a )
native 'swap', swap, 0
  pop rax ; stack top
  pop rbx ; stack top - 1
  push rax
  push rbx ; new stack top
endnative

; ( a b -- a b a )
native 'over', over, 0
  push qword [rsp + 8]
endnative

; ( a -- a a )
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
