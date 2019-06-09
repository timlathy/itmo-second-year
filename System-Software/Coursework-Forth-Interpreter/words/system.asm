; vim: syntax=nasm

native '.', dot, 0
  pop rdi
  call native_print_int
  call native_print_newline
endnative

native '.S', dotstack, 0
  mov [rsp - 8], rbx ; save (push) rbx without changing the stack pointer
  mov rbx, rsp
dotstack_loop:
  cmp rbx, [stack_start_ptr]
  je dotstack_ret
  mov rdi, [rbx]
  add rbx, 8
  call native_print_int
  call native_print_newline
  jmp dotstack_loop
dotstack_ret:
  mov rbx, [rsp - 8] ; restore rbx
endnative

native 'emit', emit, 0
  pop rdi
  call native_print_char
endnative

native 'word', word, 0
  mov rdi, input_scratch
  mov rsi, max_word_length
  call native_read_word
  push rax ; str
  push rdx ; str-len
endnative

native 'exit', exit, 0
  mov pc, [rstack]
  add rstack, 8
endnative

native 'bye', bye, 0
  call native_exit
