#! /bin/bash
# Uses run-clang-tidy.py, so clang-tidy must be in your $PATH for this script to run
cd ..
rm -rf build/
mkdir build && cd build
cmake ..
`locate run-clang-tidy.py` -fix