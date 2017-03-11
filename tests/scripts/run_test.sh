#!/bin/bash

if [[ ! "$(pwd)" =~ scripts$ ]]
then
    printf "You need to be in the scripts directory to run this script!";
    exit 1;
fi

USE_MASSIF="yes"
USE_CALLGRIND="yes"

#Parser all arguments and put them into variables
for i in "$@"
do
    case $i in
        --test-name=*)
        TEST_NAME="${i#*=}"
        shift # past argument=value
        ;;
        --results-folder=*)
        RESULTS_FOLDER="${i#*=}"
        shift # past argument=value
        ;;
        --command=*)
        COMMAND="${i#*=}"
        shift # past argument=value
        ;;
        --use-massif=*)
        USE_MASSIF="${i#*=}"
        shift # past argument=value
        ;;
        --use-callgrind=*)
        USE_CALLGRIND="${i#*=}"
        shift # past argument=value
        ;;
    esac
done

#Check if all nessecarry arguments were set
if [[ "$TEST_NAME" == "" ]];
then
    printf "Test needs to be named. --test-name=<test name>\n";
    exit 1;
fi

if [[ "$RESULTS_FOLDER" == "" ]];
then
    printf "Results folder path needs to be specified. --results-folder=<folder path>\n";  
    exit 1;
fi

if [[ "$COMMAND" == "" ]];
then
    printf "Command needs to be specified. --command=<command>\n";  
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
if [[ "$USE_MASSIF" == "yes" ]];
then
    valgrind --tool=massif --time-unit=B --detailed-freq=100 --massif-out-file=massif_output.txt --log-file=massif_log.txt $COMMAND > /dev/null 2>&1;
fi

if [[ "$USE_CALLGRIND" == "yes" ]];
then
    valgrind --tool=callgrind --callgrind-out-file=callgrind_output.txt --log-file=callgrind_log.txt --cache-sim=yes --branch-sim=yes $COMMAND > /dev/null 2>&1;
fi


#Do time estimate of program
{ time $COMMAND > program_output.txt 2> program_error_output.txt ; } 2> time_output.txt;

mkdir -p $RESULTS_FOLDER;


#Set folder name using date and time
folderName="_$(date +%F_%H-%M-%S)";
folderName="$TEST_NAME$folderName";
folderPath="$RESULTS_FOLDER/$folderName";


mkdir $folderPath


#Move all outputs and logs to the appropriate folder
if [[ "$USE_MASSIF" == "yes" ]];
then
    mv massif_log.txt $folderPath/
    mv massif_output.txt $folderPath/
fi

if [[ "$USE_CALLGRIND" == "yes" ]];
then
    mv callgrind_log.txt $folderPath/
    mv callgrind_output.txt $folderPath/
fi

mv program_output.txt $folderPath/
mv program_error_output.txt $folderPath/
mv time_output.txt $folderPath/


echo "$hardwareInfo" >> $folderPath/hardware_info.txt;
echo "$hardwareInfoVerbose" >> $folderPath/hardware_info_verbose.txt;
echo "$cpuInfo" >> $folderPath/cpu_info.txt;
echo "$batteryInfo" >> $folderPath/battery_info.txt;
echo "$cMakeCache" >> $folderPath/cmakecache_copy.txt;


popd > /dev/null 2>&1;
