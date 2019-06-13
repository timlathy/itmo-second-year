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

\ Indefinite loops
\ * begin [condition-body] while [loop-body] repeat
\     condition-body leaves a flag on the stack;
\     while pops it off, and if it's truthy, proceeds to loop-body, otherwise skips it

: begin
    here \ push condition-body address
; immediate

: while
    lit 0branch , \ check the result of condition-body
    here \ push the address of 0branch target (later replaced with the loop end address)
    0 , \ reserve it
; immediate

: repeat
    lit branch , \ unconditionally branch
    swap ,       \ to condition-body (the first address we've pushed on the stack)
    here swap !  \ set the target for the 0branch compiled in while to current location (first word after the loop)
; immediate

\ Definite loops
\ * [limit] [index] for [loop-body] endfor
\       if index < limit then execute loop-body, increment index, repeat; otherwise exit the loop

: for
    lit swap , lit >r , lit >r , \ insert the initialization sequence: move limit and index to the return stack, with index on top
    here                         \ after initialization, we evaluate loop condition — this is the target for branch at the end of the loop, store it!
    lit r> , lit dup , lit r@ ,  \ runtime data stack: ( index, index, limit ), return stack: ( limit )
    lit < ,                      \ ( index, index < limit? )
    lit 0branch ,                \ exit the loop. runtime data stack at this point is ( index ), return stack is ( limit )
    here 0 ,                     \ (we'll put the address of the loop terminator there)
    lit >r ,                     \ runtime data stack: ( ), return stack: ( limit index )
; immediate

: endfor
    lit r> , lit 1+ , lit >r , \ increment the index
    swap                       \ bring the address of the loop condition target to the top
    lit branch , ,             \ branch to the loop condition
    here swap !                \ set the target for the loop terminator
    lit r> , lit 2drop ,       \ loop terminator: pop the limit from the retstack and drop it along with index
; immediate
