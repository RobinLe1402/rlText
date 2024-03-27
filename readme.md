# The rlText dynamic library
<img src="/res/badges/utf8.svg" title="The code in this repo uses UTF-8 strings" width="50px"/>

This repo contains a dynamic version of the library `rlText`, it provides text encoding and decoding
functionality for the following text encodings:
* (pure) ASCII
* Windows Codepage 1252 (often erroneously called "ANSI")
* UTF-8
* UTF-16
* UTF-32

It can create files with little endian or big endian.

Functions for conversion between UTF-8 and Windows' UTF-16 `wchar_t` are provided in
`include/rlTextDLL/Windows.h`.


## C++ Wrappers
The `include` directory also contains C++ wrappers for the C headers.

`.h` files are the original C headers, `.hpp` are the C++ wrapper headers.



## Enviroment

| Category                          | Value            |
|-----------------------------------|------------------|
| C++ Language Standard (internal)  | ISO C++20 (MSVC) |
| C   Language Standard (interface) | ISO C11          |
| C++ Language Standard (interface) | ISO C++11        |

### Requirements
Both when compiling and using this library, it is assumed that your platform supports
8-, 16-, 32- and 64-bit integers (of exactly that bit count).
