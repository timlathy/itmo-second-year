; vim: syntax=nasm

msg_word_not_found: db 'Word not found', 0xa
len_word_not_found: equ $-msg_word_not_found

; === interpreter-loop ===
; Main interpreter routine. Reads and executes words until encountering EOF.

colon 'interpreter-loop', interpreter_loop
interpreter_loop:
  dq xt_word ; ( str str-len )
  dq xt_over ; ( str str-len str )
  dq xt_zbranch, interpreter_loop_eof ; str is null when an EOF is reached
  dq xt_dup ; ( str str-len str-len )
  dq xt_zbranch, interpreter_loop_empty_line
  dq xt_interpret ; ( )
  dq xt_branch, interpreter_loop
interpreter_loop_empty_line:
  dq xt_twodrop, xt_branch, interpreter_loop
interpreter_loop_eof:
  dq xt_bye

; === interpret ===
; ( str str-len -- )

colon 'interpret', interpret
  dq xt_twodup ; ( str str-len str str-len )
  dq xt_find ; ( str str-len word-header )
  dq xt_dup ; ( str str-len word-header word-header )
  dq xt_zbranch, interpret_check_number ; word-header is null => word not found
interpret_word:
  dq xt_nip, xt_nip ; ( word-header )
  dq xt_immediate_mode_q ; ( word-header immed )
  dq xt_zbranch, interpret_compile_mode
interpret_immediate:
  dq xt_cfa ; ( exec-token )
  dq xt_execute, xt_exit
interpret_compile_mode:
  dq xt_dup, xt_immediate_word_q ; ( word-header immed-flag )
  dq xt_zbranch, interpret_compile_word
  dq xt_branch, interpret_immediate
interpret_compile_word:
  dq xt_cfa ; ( exec-token )
  dq xt_comma, xt_exit ; append execution token to the current word definition
interpret_check_number: ; ( str str-len word-header )
  dq xt_drop ; ( str str-len )
  dq xt_number ; ( num parsed-len )
  dq xt_zbranch, interpret_word_not_found ; parsed-len is zero => not a number
  dq xt_immediate_mode_q, xt_zbranch, intrepret_compile_number
intrepret_immediate_number: ; ( num )
  dq xt_exit ; in immediate mode, just leave the number on top of the stack
intrepret_compile_number: ; ( num )
  dq xt_lit, xt_lit, xt_comma ; append lit to current word definition
  dq xt_comma ; write the number after lit
  dq xt_exit
interpret_word_not_found: ; ( num )
  dq xt_drop
  dq xt_lit, msg_word_not_found, xt_lit, len_word_not_found, xt_type
  dq xt_exit

; === colon ===
; Reads a word and creates a new dictionary entry for it
colon ':', colon
  dq xt_word   ; ( str str-len )
  dq xt_create ; ( )
  dq xt_lit, docol, xt_comma ; insert docol at the beginning of the word definition
  dq xt_lbrac ; enter compile mode
  dq xt_exit

; === semicolon ===
; Ends the compilation of the current word
colon ';', semicolon, 1
  dq xt_lit, xt_exit, xt_comma ; insert exit at the end of the word definition
  dq xt_rbrac ; exit compile mode
  dq xt_exit

; === type ===
; Prints cnt characters starting at the given address.
; ( addr cnt -- )
colon 'type', type, 0
type_loop:
  dq xt_dup, xt_zbranch, type_ret ; cnt is 0?
  dq xt_swap, xt_dup ; ( cnt addr addr )
  dq xt_readbyteat ; ( cnt addr char )
  dq xt_emit ; ( cnt addr )
  dq xt_incr, xt_swap, xt_decr ; ( addr+1 cnt-1 )
  dq xt_branch, type_loop
type_ret:
  dq xt_twodrop, xt_exit
