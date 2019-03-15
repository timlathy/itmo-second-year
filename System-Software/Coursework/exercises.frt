0 constant false
1 constant true

( Task 3 )

: is-even 2 % not ;
: is-odd 2 % ;

( Task 4 )

: is-prime dup 4 <
    ( n [n < 4] )
    if
        1 > ( [1 > n] )
    else
        dup is-even ( n even? )
        if ( even numbers greater than 2 are not primes )
            drop false ( false )
        else
            3 >r ( n ) ( i )
            repeat
                dup r@ % ( n [n % i] ) ( i )
                if ( n % i != 0 )
                    dup r@ dup * ( n n [i^2] ) ( i )
                    > ( n [n > i^2] ) ( i )
                    if
                        r> 2 + >r ( n ) ( [i + 2] )
                        false
                        ( n finished? )
                    else
                        true true
                        ( n prime? finished? )
                    then
                else
                    false true
                    ( n prime? finished? )
                then
            until
            ( n prime? ) ( i )
            r> drop
            ( n prime? ) ( )
            swap drop
            ( prime? ) ( )
        then
    then
;

( Task 5 )

: store-bool-char ( bool -- addr )
    1 allot ( bool addr )
    swap over ( addr bool addr )
    c! ( addr )
;

: print-bool-char ( char addr -- )
    c@ .
;

: is-prime-store-and-print-result ( n -- )
    is-prime ( prime? )
    store-bool-char ( addr )
    dup ( addr addr )
    print-bool-char ."  stored at " . cr
;

( Task 7 )

: copy-bytes ( s d count -- d-end )
    0 for
        ( s d )
        over c@ ( s d *s )
        swap ( s *s d )
        dup >r c! r> ( s d )
        2inc ( s+1 d+1 )
    endfor
    swap drop
;

: concat ( str1 str2 -- str3 )
    dup count rot dup count ( str2 str2-len str1 str1-len )
    rot ( str2 str1 str1-len str2-len )
    2dup + ( str2 str1 str1-len srt2-len str3-len )
    1 + ( leave a byte for the null terminator )
    heap-alloc ( str2 str1 str1-len srt2-len str3 )
    >r ( str2 str1 str1-len str2-len ) ( str3 )
    rot ( str2 str1-len str2-len str1 ) ( str3 )
    rot ( str2 str2-len str1 str1-len ) ( str3 )
    r@ ( str2 str2-len str1 str1-len str3 ) ( str3 )
    rot ( str2 str2-len str1-len str3 str1 ) ( str3 )
    swap ( str2 str2-len str1-len str1 str3 ) ( str3 )
    rot ( str2 str2-len str1 str3 str1-len ) ( str3 )
    copy-bytes ( str2 str2-len str3-after-str1 ) ( str3 )
    swap ( str2 str3-after-str1 str2-len ) ( str3 )
    copy-bytes ( str3-after-str2 ) ( str3 )
    0 swap
    c! ( write the null terminator )
    r> ( str3 ) ( )
;

( Task 8 )

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
