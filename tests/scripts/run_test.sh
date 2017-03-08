#!/bin/bash

if [[ ! "$(pwd)" =~ scripts$ ]]
then
    printf "You need to be in the scripts directory to run this script!";
    exit 1;
fi


pushd ../.. > /dev/null 2>&1;

#Getting various hardware information
hardwareInfo="$(sudo lshw -short)";
hardwareInfoVerbose="$(sudo lshw)";
cpuInfo="$(sudo lscpu)";
batteryInfo="$(upower -d)";

#Copying cmake cache
cMakeCache="$(cat build/CMakeCache.txt)"

#Running valgrind programs
valgrind --tool=massif --time-unit=ms --detailed-freq=1 --massif-out-file=massif_output.txt --log-file=massif_log.txt $*
valgrind --tool=callgrind --callgrind-out-file=callgrind_output.txt --log-file=callgrind_log.txt --cache-sim=yes --branch-sim=yes $*

#Do time estimate of program
{ time $* > program_output.txt ; } 2> time_output.txt;

popd > /dev/null 2>&1;


pushd .. > /dev/null 2>&1;

#Create new results folder if there are none
if [[ ! -d results ]]
then
    mkdir results;
fi

pushd results > /dev/null 2>&1;

#Set folder name using date and time
folderName="$(date +%F_%H-%M-%S)"

mkdir "$folderName"

pushd "$folderName" > /dev/null 2>&1

#Move all outputs and logs to the appropriate folder
mv ../../../massif_log.txt ./
mv ../../../callgrind_log.txt ./
mv ../../../massif_output.txt ./
mv ../../../callgrind_output.txt ./
mv ../../../program_output.txt ./
mv ../../../time_output.txt ./


echo "$hardwareInfo" >> hardware_info.txt;
echo "$hardwareInfoVerbose" >> hardware_info_verbose.txt;
echo "$cpuInfo" >> cpu_info.txt;
echo "$batteryInfo" >> battery_info.txt;
echo "$cMakeCache" >> cmakecache_copy.txt;


popd > /dev/null 2>&1;
