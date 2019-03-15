# Tests my Forth implementation of primality test

import numpy
import subprocess

FORTH_WORD = 'is-prime'
PATH_TO_FRT = 'exercises.frt'

# https://stackoverflow.com/a/3035188
def primes(n):
    sieve = numpy.ones(n//2, dtype=numpy.bool)
    for i in range(3, int(n**0.5) + 1, 2):
        if sieve[i//2]:
            sieve[i*i//2::i] = False
    return 2*numpy.nonzero(sieve)[0][1::] + 1

primes_upto_1000 = [2] + list(primes(1000))

for i in range(0, 1000):
    cmd = f'cd forthress; echo "{i} {FORTH_WORD} . cr" | cat ../{PATH_TO_FRT} - | ./start'
    ps = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    output = ps.communicate()[0]
    res = output.split(b'\n')[1]

    if i in primes_upto_1000:
        if res == b'1':
            print(f'{i} passed (prime)')
        else:
            print(f'{i} failed (is a prime, marked as non-prime)')
            break
    else:
        if res == b'0':
            print(f'{i} passed (non-prime)')
        else:
            print(f'{i} failed (is not a prime, marked as prime)')
            break
