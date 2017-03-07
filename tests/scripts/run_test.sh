#!/bin/bash

if [[ ! "$(pwd)" =~ scripts$ ]]
then
    printf "You need to be in the scripts directory to run this script!";
    exit 1;
fi


pushd ../.. > /dev/null 2>&1;

hardwareInfo="$(sudo lshw -short)";
hardwareInfoVerbose="$(sudo lshw)";
cpuInfo="$(sudo lscpu)";
batteryInfo="$(upower -d)";
cmakeCache="$(cat build/CMakeCache.txt)"
#timingInfo="$(/usr/bin/time --format="Test input: %C\nDuraction in seconds: %e\n" $*)";

startTime=$(date +%s.%N);
programOutput="$($*)";
endTime=$(date +%s.%N);
duration="Duration in seconds: $(echo "$endTime - $startTime" | bc)";

printf "$programOutput\n";

popd > /dev/null 2>&1;


pushd .. > /dev/null 2>&1;

if [[ ! -d results ]]
then
    mkdir results;
fi

pushd results > /dev/null 2>&1;

folderName="$(date +%F_%H-%M-%S)"

mkdir "$folderName"

pushd "$folderName" > /dev/null 2>&1


echo "$hardwareInfo" >> hardwareInfoShort.txt
echo "$hardwareInfoVerbose" >> hardwareInfoLong.txt
echo "$cpuInfo" >> cpuInfo.txt
echo "$batteryInfo" >> batteryInfo.txt
echo "$programOutput" >> programOutput.txt
echo "$cmakeCache" >> cmakeCache.txt
echo "$duration" >> duration.txt


popd > /dev/null 2>&1;
