#!/bin/bash

pushd ../.. > /dev/null 2>&1

output="$(./build/bin/thread_safe_logger -thread_amount 4 -run_time_ms 1000 --logall 2>&1)";

popd > /dev/null 2>&1

testString="ABCDEFGHIJKLMNOPQRSTUVWXYZ";

IFS=$'\n';
for line in $output;
do
    if [ "$line" != "$testString" ];
    then
        printf "Test failed!\n";  
        printf "Line contents: $line\n";
        exit 1;
    fi
done

printf "Test successfull!\n";
