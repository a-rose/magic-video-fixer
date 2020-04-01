#!/bin/bash

# Quick Build script for developers in a hurry ;)

BUILD_DIR="build"
INSTALL_DIR=".."

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_INSTALL_RPATH="${BUILD_DIR}/lib"
make -j4 && echo -e "\nStart with ./${BUILD_DIR}/magic-video-fixer"
