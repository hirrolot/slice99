#!/bin/bash

clang tests.c -I. -Weverything -std=c99 -o tests
./tests
rm tests
