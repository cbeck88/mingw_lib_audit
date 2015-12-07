#!/bin/bash
set -e

export DEP_ROOT=`pwd`/mingw

rm -rf build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE="../Toolchain-mingw.cmake" -DCMAKE_INSTALL_PREFIX="$DEP_ROOT" ..
make VERBOSE=1
cd ..

# Copy stuff, assets and bin
cp assets/* build/
cp mingw/bin/* build/
