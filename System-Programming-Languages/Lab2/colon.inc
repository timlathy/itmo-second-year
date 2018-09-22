; vim: syntax=nasm

; A _word_ is an element of a linked list structure with a pointer
; to the previous element, a null-terminated name string, and an arbitrary
; blob of contents.
;
; Here's the structure, visualized:
; | 8-byte tail pointer | null-terminated word name | word contents
;
; The tail pointer and the name of the word together make up the _word header_.

; ===== MACRO colon =====
;
; Inserts a word header, to be followed up by word contents.
; Accepts two arguments, a word name string (without the NULL terminator) and
; an assembly label inserted before the word contents, i.e. after the head.

%define colon_head 0 ; colon_head stores the current head of the dictionary

%macro colon 2
%%word_head: dq colon_head     ; %%word_head is unique for every macro invocation
db %1, 0                       ; null-terminated word name
%2:                            ; an assembly label for word contents

%define colon_head %%word_head ; %%word_head will point to _this_ word's header
%endmacro

colon "head", word_head
db "Mami Tomoe", 0

colon "madoka", word_madoka
db "Homura Akemi", 0
