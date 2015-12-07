#!/bin/bash
set -e

LIB_KIT_DIR=~/mingw_lib_kit

cp -v ${LIB_KIT_DIR}/mingw_dep_pack.tar.gz .
cp -v ${LIB_KIT_DIR}/Toolchain-mingw.cmake .

sleep 1

rm -rf mingw
tar -xzvf mingw_dep_pack.tar.gz
