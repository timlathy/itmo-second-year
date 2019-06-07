; vim: syntax=nasm

%define max_word_length 64

%define __dict_last_word__ 0

; Forth dictionary:
; The interpreter looks up words in a dictionary — a linked list of word headers (WHs).

; Native words:

%macro native 3 ; %1 = word name (string), %2 = word label, %3 = immediate flag
  %strlen __dict_name_len__ %1

  section .data
  wh_%+ %2: dq __dict_last_word__  ; qword: pointer to the previous word (0 for the first word)
  db (__dict_name_len__ << 1) | %3 ; byte: word name length shifted by 1 and immediate flag in the lower bit
  db %1                            ; word name (up to 255 characters since the length is stored in a byte)

  %define __dict_last_word__ wh_%+ %2

  xt_%+ %2: ; xt_... is the execution token — a pointer to the machine code implementation of the word
  dq i_%+ %2
  section .text
  i_%+ %2:
%endmacro

%macro endnative 0
  jmp next ; return to the interpreter
%endmacro

; Non-native words consist of execution tokens only. The first token always points to _docol_.

%macro colon 2
  %strlen __dict_name_len__ %1

  section .data
  wh_%+ %2: dq __dict_last_word__
  db (__dict_name_len__ << 1) ; immediate flag (in the lower bit) is always 0
  db %1

  %define __dict_last_word__ wh_%+ %2

  xt_%+ %2:
  dq docol
%endmacro
