; vim: syntax=nasm

; Dictionary entries are called _words_, referencing
; the basic data structure of the Forth programming language.
;
; Each _word_ consists of a pointer to the previous element,
; a null-terminated name string, and an arbitrary value.
;
; Here's the structure, visualized:
; | 8-byte tail pointer | null-terminated word name | word contents
;
; The tail pointer and the name of the word together make up the _word header_.

; ===== colon =====
;
; Inserts a word header, to be followed up by word contents.
; Accepts two arguments: a word name string (without the NULL terminator) and
; an assembly label inserted before the word contents, i.e. after the head.

%define colon_head 0 ; colon_head is the current head of the dictionary,
                     ; i.e. the pointer to the last defined element

%macro colon 2
%%word_head: dq colon_head     ; %%word_head is unique for every macro invocation
db %1, 0                       ; null-terminate the word name string
%2:                            ; insert an assembly label for word contents

%define colon_head %%word_head ; %%word_head will point to _this_ word's header
%endmacro

; =====
; A couple of words are defined to test the implementation:

colon "head", word_head
db "Mami Tomoe", 0

colon "madoka", word_madoka
db "Homura Akemi", 0
