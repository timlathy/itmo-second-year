include ../numeric.frt

: collatz-step ( n )
    dup is-odd
    if
        3 * 1 + ( [3n + 1] )
    else
        2 / ( [n / 2] )
    then
;

: collatz ( n )
    dup . cr
    dup 1 >
    if
        repeat
            collatz-step
            dup . cr
            dup 1 >
        not until
    then
;
