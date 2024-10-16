#!/bin/bash

ROOT="$(dirname $(realpath ${BASH_SOURCE[0]}))/.."
pushd $ROOT

cmake -B build
make --directory=build

popd
