#!/bin/bash

clang tests.c -I. -Weverything -std=c99 -o tests
./tests
rm tests

mkdir -p examples/build
cd examples/build
cmake ..
cmake --build .
./demo
cd ../..

cppcheck tests.c
scan-build --use-analyzer=/usr/bin/clang tests.c
