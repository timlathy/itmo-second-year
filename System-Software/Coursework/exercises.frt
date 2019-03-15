include ../collatz.frt

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
