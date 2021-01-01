#!/bin/bash

# Check the tests.
cppcheck tests.c
scan-build --use-analyzer=/usr/bin/clang clang tests.c -I. -Iassert-algebraic -Weverything -std=c99 -o tests
./tests
rm tests

# Check the examples.
mkdir -p examples/build
cd examples/build
cmake ..
cmake --build .
./demo
cd ../..
