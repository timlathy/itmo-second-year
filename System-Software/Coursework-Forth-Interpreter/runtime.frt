\ Conditionals
\ * [condition] if [true-body] then
\ * [condition] if [true-body] else [false-body] then

: if
    lit 0branch , \ compile 0branch (checks the value currently on the stack)
    here          \ store the address of the qword after 0branch, which specifies branch target (we'll later replace it)
    0 ,           \ reserve a qword for the target, advancing HERE pointer
; immediate

: then
    here   \ get current address — the first word after the body of if-then
    swap ! \ set it as the 0branch target
; immediate

: else
    lit branch , \ insert an unconditional branch after true-body (if it's been executed, we skip false-body) 
    here         \ store the address containing branch target
    0 ,          \ reserve it
    swap         \ now comes the interesting part: retrieve the address of the 0branch target preceding the if body
    here swap !  \ and store the current address (start of false-body) there => when the if condition fails we branch to else
; immediate

