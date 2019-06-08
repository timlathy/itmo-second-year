# Minimal Forth Intrepreter, x86-64, Linux syscall ABI

_Work in progress_

[forthress](https://github.com/sayon/forthress) is used as a reference implementation.
Main differences come from the approach I took to string handling. The buffer is not
null-terminated (C style), rather, all strings are passed as two `qword`s: length
and buffer pointer.

## Development

```sh
make
gdb forth
```

Placing `int3` instructions instead of setting breakpoints explicitly in a GDB
sessions saves time.
