#!/bin/bash

mkdir -p examples/build
cd examples/build
cmake ..
cmake --build .
./demo
cd ../..
