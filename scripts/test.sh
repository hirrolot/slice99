#!/bin/bash

cppcheck tests.c

# Test Clang with scan-build.
scan-build --use-analyzer=/usr/bin/clang clang tests.c -I. -Iassert-algebraic -Weverything -std=c99 -o tests -fsanitize=address
./tests
rm tests

# Test GCC.
gcc tests.c -I. -Iassert-algebraic -Wall -Wextra -pedantic -std=c99 -o tests -fsanitize=address
./tests
rm tests

# Check the examples.
mkdir -p examples/build
cd examples/build
cmake ..
cmake --build .
./demo
cd ../..
