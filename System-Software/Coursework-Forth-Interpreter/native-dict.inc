; vim: syntax=nasm

native 'lit', lit, 0
  push qword [pc]
  add pc, 8
endnative

; stack operations

native 'drop', drop, 0
  pop rax
endnative

native 'swap', swap, 0
  pop rax ; stack top
  pop rbx ; stack top - 1
  push rax
  push rbx ; new stack top
endnative

native 'dup', dup, 0
  push qword [rsp]
endnative

native '.', dot, 0
  pop rdi
  call native_print_int
endnative

; math

native '+', plus, 0
  pop rax
  add [rsp], rax
endnative

native '-', minus, 0
  pop rax
  sub [rsp], rax
endnative

; dictionary

; ( str str-len -- word-header-ptr )
native 'find', find, 0
  push rbx ; rbx = string length
  push rsi ; rsi = string
  mov rax, [dict_last_word]

find_loop:
  test rax, rax     ; rax is the pointer to a word header
  jz find_not_found ; 0 (null) => we've traversed the whole dictionary
                    ; but did not find the word we're looking for
  mov cl, [rax + 8] ; the first 8 bytes of WH are occupied by the next word
                    ; pointer, the byte after it is the word name length
  cmp rcx, rbx      ; are name lengths equal?
  jne find_loop_next
find_loop_cmp_loop:
  mov rdx, [rsi + rcx]
  cmp [rax + 8 + 1 + rcx], rdx ; base + next ptr (8) + name length (1) = name start
  jne find_loop_next
  inc rcx
  cmp rcx, rbx
  jne find_loop_cmp_loop
find_loop_found:
  push rax
  endnative
find_loop_next:
  mov rax, [rax]
  jmp find_loop  
find_not_found:
  push qword 0
  endnative

; system

native 'word', word, 0
  mov rdi, input_scratch
  mov rsi, max_word_length
  call native_read_word
  push input_scratch
endnative

native 'exit', exit, 0
  mov pc, [rstack]
  add rstack, 8
endnative

native 'bye', bye, 0
  call native_exit
