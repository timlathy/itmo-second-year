: if
    lit 0branch ,
    here
    0 ,
; immediate

: then
    here
    swap !
; immediate

: else
    lit branch ,
    here
    0 ,
    swap
    here swap !
; immediate
