; vim: syntax=nasm

%define __dict_prev_word__ 0

; Forth dictionary:
; The interpreter looks up words in a dictionary — a linked list of word headers (WHs).

; Native words:

%macro native 3 ; %1 = word name (string), %2 = word label, %3 = immediate flag
  %strlen __dict_name_len__ %1

  section .data
  wh_%+ %2: dq __dict_prev_word__ ; pointer to the previous word (0 for the first word)
  db __dict_name_len__ ; word name length
  db %1 ; word name (up to 255 characters since the length is stored in a byte)
  db %3 ; immediate flag
  
  %define __dict_prev_word__ wh_%+ %2

  xt_%+ %2: ; xt_... is the execution token — a pointer to the machine code implementation of the word
  dq i_%+ %2
  section .text
  i_%+ %2:
%endmacro

; Non-native words consist of execution tokens only. The first token always points to _docol_.

%macro colon 2
  %strlen __dict_name_len__ %1

  section .data
  wh_%+ %2: dq __dict_prev_word__
  db __dict_name_len__
  db %1
  db 0
  
  %define __dict_prev_word__ wh_%+ %2

  xt_%+ %2:
  dq docol
%endmacro
