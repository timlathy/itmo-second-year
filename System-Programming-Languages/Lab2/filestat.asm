; vim: syntax=nasm

; Compile with
; nasm -f elf64 filestat.asm && ld -o filestat filestat.o lib.o

global _start
extern print_uint
extern print_string
extern print_newline
extern exit
extern panic

section .data
msg_file_size: db "File size (bytes): ", 0
msg_contents: db "Contents: ", 10, 0
msg_invalid_args: db "filestat accepts a single argument, path to the target file", 10, 0
msg_syscall_error: db "An error occurred during a system call", 10, 0

; Memory layout of the struct filled in by sys_newstat():
;
;   struct    stat:       sizeof = 144
;   dev_t     st_dev:     sizeof = 8, offset = 0
;   ino_t     st_ino:     sizeof = 8, offset = 8
;   mode_t    st_mode:    sizeof = 4, offset = 24
;   nlink_t   st_nlink:   sizeof = 8, offset = 16
;   uid_t     st_uid:     sizeof = 4, offset = 28
;   gid_t     st_gid:     sizeof = 4, offset = 32
;   dev_t     st_rdev:    sizeof = 8, offset = 40
;   off_t     st_size:    sizeof = 8, offset = 48
;   blksize_t st_blksize: sizeof = 8, offset = 56
;   blkcnt_t  st_blocks:  sizeof = 8, offset = 64
;
; (Obtained using stat_layout.c)

%define FD_STDOUT 1
%define SYS_WRITE 1
%define SYS_OPEN 2
%define SYS_NEWSTAT 4
%define SYS_MMAP 9
%define STAT_STRUCT_SIZE_OFFSET 48

section .text

_start:
  cmp qword [rsp], 2   ; the top of the stack is a pointer to argc
                       ; since the first argument is always the executable name,
                       ; we need two     
  jne invalid_args
get_file_size:
  mov rbx, rsp         ; save rbx to restore it later
  sub rsp, 144         ; reserve space for the stat struct (see above) on the stack
  mov rsi, rsp         ; statbuf = rsp
  mov rdi, [rbx + 16]  ; the second argv is a pointer to the command line argument,
                       ; which in our case is the name of the file
  mov rax, SYS_NEWSTAT
  syscall
  test rax, rax        ; on success, stat() returns 0
  jnz syscall_error
  mov r12, [rsp + STAT_STRUCT_SIZE_OFFSET] 
print_file_size:
  mov rdi, msg_file_size
  call print_string
  mov rdi, r12
  call print_uint
  call print_newline
  mov rsp, rbx         ; restore rsp (delete the stat struct)
get_file_descriptor:
  mov rdi, [rsp + 16]  ; filename
  mov rsi, 0           ; read-only
  mov rax, SYS_OPEN
  syscall
  cmp rax, -1          ; on error, open() returns -1
  je syscall_error
load_file_contents:
  mov rdi, 0           ; let the kernel choose the address
  mov rsi, r12         ; length = file size
  mov rdx, 1           ; MAP_PRIVATE
  mov r10, 2           ; PROT_READ
  mov r8, rax          ; open() returned the file descriptor in rax
  mov r9, 0            ; offset into the file
  mov rax, SYS_MMAP
  syscall
  cmp rax, -1
  je syscall_error
  mov r13, rax         ; r13 now points to the file contents buffer
print_file_contents:
  mov rdi, msg_contents
  call print_string
  mov rdx, r12         ; length
  mov rsi, r13         ; file contents buffer
  mov rdi, FD_STDOUT
  mov rax, SYS_WRITE
  syscall
  call exit

syscall_error:
  mov rdi, msg_syscall_error
  call panic
invalid_args:
  mov rdi, msg_invalid_args
  call panic
