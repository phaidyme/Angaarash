#!/bin/bash
# This is meant to be run from the root of the repository.

if [ -z $1 ]
  then
    echo "Please provide a valid install destination."
    exit 1
fi

cmake -S . -B build -D CMAKE_INSTALL_PREFIX=$1
cmake --build build
cmake --install build