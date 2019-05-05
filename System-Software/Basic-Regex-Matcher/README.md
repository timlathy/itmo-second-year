# Basic Regex Matcher

## Prerequisites

* [opam](http://opam.ocaml.org/doc/Install.html)
* `clang`, `clang-format`
* `libffi-devel`

```sh
opam install -y dune base ctypes ctypes-foreign ounit
```

## Building

```sh
dune build main.exe
./_build/default/main.exe
```

## Running tests

```sh
dune runtest
```
