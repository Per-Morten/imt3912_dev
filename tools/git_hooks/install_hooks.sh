#!/bin/bash

echo "Copying files"

pushd ../../.git/hooks > /dev/null 2>&1
cp ../../tools/git_hooks/commit-msg ./
popd > /dev/null 2>&1

echo "Done"
