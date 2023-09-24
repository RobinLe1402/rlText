# The rlText dynamic library

This repo contains a dynamic version of the library `rlText`, it provides text encoding and decoding
functionality for the following text encodings:
* (pure) ASCII
* Windows Codepage 1252 (often erroneously called "ANSI")
* UTF-8
* UTF-16
* UTF-32

It can create files with little endian or big endian.

The encoding used internally and for interfacing with applications is UTF-8
(however, functions for conversion from and to Windows' UTF-16 `wchar_t` are also provided in
`include/rlTextDLL/Windows.h`).



## Enviroment

| Category                          | Value            |
|-----------------------------------|------------------|
| C++ Language Standard (internal)  | ISO C++20 (MSVC) |
| C   Language Standard (interface) | ISO C11          |
| C++ Language Standard (interface) | ISO C++11        |

### Requirements
Both when compiling and using this library, it is assumed that your platform supports
8-, 16-, 32- and 64-bit integers (of exactly that bit count).