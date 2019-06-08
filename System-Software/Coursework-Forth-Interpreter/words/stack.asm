; vim: syntax=nasm

; ( a b -- a )
native 'drop', drop, 0
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
