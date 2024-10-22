#!/bin/bash

ROOT="$(dirname $(realpath ${BASH_SOURCE[0]}))/.."
pushd $ROOT

#cmake --build build/ --target clean
rm -rf build/*

popd


