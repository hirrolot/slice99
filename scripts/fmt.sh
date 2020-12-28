#!/bin/bash

find slice99.h examples tests.c -iname *.h -o -iname *.c -path examples/build -prune | xargs clang-format -i
