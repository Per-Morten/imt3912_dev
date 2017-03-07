#!/bin/bash

if [[ ! "$(pwd)" =~ compilation_time$ ]]
then
    printf "You need to be in the compilation_time folder to run this script!\n";
    exit 1;
fi

pushd ../.. > /dev/null 2>&1

if [ -d "build" ]
then
    rm -rf build
fi

mkdir build;

pushd build > /dev/null 2>&1

cmake ..
make

popd > /dev/null 2>&1
popd > /dev/null 2>&1
