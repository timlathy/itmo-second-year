; vim: syntax=nasm

; === interpreter-loop ===
; Main interpreter routine. Reads and executes words until encountering EOF.

colon 'interpreter-loop', interpreter_loop
interpreter_loop:
  dq xt_word ; ( str str-len )
  dq xt_dup ; ( str str-len str-len )
  dq xt_zbranch, interpreter_loop_eof ; ( FIXME: this checks for any empty string, which can also occur when the user moves to a new line )
  dq xt_interpret ; ( )
  dq xt_branch, interpreter_loop
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
  dq xt_cfa ; ( str str-len exec-token )

  ; TODO: check immediate flag & current mode

  dq xt_nip, xt_nip ; ( exec-token )
  dq xt_execute
  dq xt_exit 

interpret_check_number: ; ( str str-len word-header )
  dq xt_drop ; ( str str-len )
  dq xt_number ; ( num parsed-len )
  dq xt_zbranch, interpret_word_not_found ; parsed-len is zero => not a number

interpret_word_not_found: ; ( num )
  dq xt_drop

  ; TODO: print error message
