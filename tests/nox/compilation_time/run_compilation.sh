#!/bin/bash

if [ -d "build" ]
then
    rm -rf build;
fi

mkdir build;

pushd build > /dev/null 2>&1;

cmake -DCOMPILATION_TEST=ON ..;
make;

popd > /dev/null 2>&1;
