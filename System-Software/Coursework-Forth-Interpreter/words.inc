; vim: syntax=nasm

colon 'interpret', interpret
  dq xt_word ; ( str str-len )
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
