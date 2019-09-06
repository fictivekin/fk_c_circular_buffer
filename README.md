# Lager Circular Buffer

Lager Circular Buffer is a circular buffer implementation extracted from [Lager](https://lagerdata.com)

This is a module for turning a static array into a circular buffer, suitable for use on embedded systems (no dynamic allocations are performed; a caller-supplied `memcpy` is used; no dependencies other than a C89 compiler and standard library). Data is never overwritten; attempting to add data to an already-full buffer will return an error.

See <https://fictivekin.github.io/fk_c_circular_buffer/> for documentation.

## Install
Simply copy `fk_circular_buffer.c` and `fk_circular_buffer.h` into your source tree and add them to your build tool.

## Run tests
`make test`

## Run coverage
`make coverage` and inspect `fk_circular_buffer.c.gcov`
