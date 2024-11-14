#!/bin/bash

./run-clang-format/run-clang-format.py \
    --exclude examples/build \
    --exclude tests/build \
    -r slice99.h examples tests
