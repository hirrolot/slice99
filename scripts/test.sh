#!/bin/bash

export COMMON_OPTIONS="-I. -Iassert-algebraic -std=c99 -o tests -fsanitize=address -Wno-padded -Werror"

echo "Testing Cppcheck"
cppcheck tests.c

echo "Testing Clang..."
scan-build --use-analyzer=/usr/bin/clang clang tests.c ${COMMON_OPTIONS} -Weverything
./tests

echo "Testing GCC..."
gcc tests.c ${COMMON_OPTIONS} -Wall -Wextra -pedantic
./tests

rm tests

echo "Testing the examples..."
mkdir -p examples/build
cd examples/build
cmake ..
cmake --build .
./demo
cd ../..
