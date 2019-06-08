; vim: syntax=nasm

; ( str str-len -- num parsed-str-len )
native 'number', number, 0
  pop rsi ; rsi <- string length
  pop rdi ; rdi <- string
  call parse_int
  push rax ; number
  push rdx ; parsed-str-len
endnative
