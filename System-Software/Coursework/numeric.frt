0 constant false
1 constant true

: is-even 2 % not ;
: is-odd 2 % ;

: is-prime dup 4 <
    ( n [n < 4] )
    if
        1 >
        ( [1 > n] )
    else
        dup is-even
        ( n even? )
        if
            drop false ( even numbers greater than 2 are not primes )
            ( false )
        else
            3 >r
            ( n ) ( i )
            repeat
                dup r@ %
                ( n [n % i] ) ( i )
                if ( n % i != 0 )
                    dup r@ dup *
                    ( n n [i^2] ) ( i )
                    > 
                    ( n [n > i^2] ) ( i )
                    if
                        r> 2 + >r
                        ( n ) ( [i + 2] )
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
