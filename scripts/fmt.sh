#!/bin/bash

find slice99.h examples -iname *.h -o -iname *.c -path examples/build -prune | xargs clang-format -i
