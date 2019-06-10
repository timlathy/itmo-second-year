# Minimal Forth Intrepreter, x86-64, Linux syscall ABI

[forthress](https://github.com/sayon/forthress) is used as a reference implementation.
Main differences come from the approach I took to string handling. The buffer is not
null-terminated (C style), rather, all strings are passed as two `qword`s: length
and buffer pointer.

## Running the interpreter

```sh
make
./forth
```

To include the runtime:

```sh
cat runtime.frt - | ./forth
```

## Examples

Print `HELLO`:

```forth
72 dp c! 69 dp 1+ c! 76 dp 2 + c! 76 dp 3 + c! 79 dp 4 + c! 10 dp 5 + c! dp 6 type
```

Find the absolute difference of two numbers (requires the runtime):

```forth
: diff-abs 2dup < if swap then - ;
2 7 diff-abs
```

## Native words

* `words/stack.asm`: stack operations (`drop`, `dup`, `swap`, `over`, `nip`, `2dup`, `2drop`, `>r`, `r>`, `r@`)
* `words/math.asm`: comparison (`=`, `<`, `<=`), logic (`not`), bitwise (`and`, `or`), arithmetic (`+`, `-`, `*`, `/`, `%`, `1+`, `1-`)
* `words/memory.asm`: user memory pointer (`dp`), memory operations (`@`, `!`, `c@`, `c!`)
* `words/strings.asm`: `number`
* `words/system.asm`: `.`, `.S`, `bye`, and other system words
* `words/interpreter.asm`: dictionary operations, branching, etc.

Core assembly routines (IO, numeric conversions) are located in the `native/` directory.
