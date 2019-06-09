; vim: syntax=nasm

native 'dp', dataptr, 0
  push qword [MEM]
endnative

; ( addr -- value )
; Fetches a qword value from the given address.
native '@', readat, 0
  pop rax
  push qword [rax] 
endnative

; ( value addr -- )
; Stores the specified qword value at the given address.
native '!', writeat, 0
  pop rax
  pop rdx
  mov [rax], rdx
endnative

; Regarding byte operations:
; To simplify the implementation, all values on the data stack are qwords.
; Storing a byte to memory pops a qword off the stack and disregards the upper 56 bits.
; Retrieving a byte from memory zero-extends it to 64 bits.

; ( addr -- value )
; Fetches a byte value from the given address.
native 'c@', readbyteat, 0
  pop rax
  movzx eax, byte [rax]
  push rax
endnative

; ( value addr -- )
; Stores the specified byte value at the given address.
native 'c!', writebyteat, 0
  pop rax
  pop rdx
  mov [rax], dl
endnative
