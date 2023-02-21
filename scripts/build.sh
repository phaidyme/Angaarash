#!/bin/bash
# This is meant to be run from the root of the repository.

CMAKE_INSTALL_PREFIX=$1

if [ -z $CMAKE_INSTALL_PREFIX ]
  then
    echo "Please provide a valid install destination."
    exit 1
fi

cmake -S . -B build
cmake --build build
cmake --install build --prefix $CMAKE_INSTALL_PREFIX