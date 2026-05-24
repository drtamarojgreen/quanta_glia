#!/bin/bash
set -e
cd "$(dirname "$0")"
# Ensure the build directory exists and is populated
mkdir -p ../../build
cd ../../build
cmake .. > /dev/null
make sorrel_test > /dev/null
cd ../tests/sdd
./../../build/sorrel_test
echo "SDD SUITE PASSED"
