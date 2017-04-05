#!/bin/bash

root="../../.."

if [[ ! "$(pwd)" =~ scripts$ ]]
then
    printf "You need to be in the scripts directory to run this script!";
    exit 1;
fi

#Default values for some arguments
USE_MASSIF="yes"
USE_CALLGRIND="yes"
SAVE_PROGRAM_OUTPUT="yes"

#Parses all arguments and put them into variables
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
        --save-program-output=*)
        SAVE_PROGRAM_OUTPUT="${1#*=}"
        shift # past argument=value
        ;;
    esac
done

#Check if all necessary arguments were set
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

pushd $root > /dev/null 2>&1;

#Set folder name using date and time
folderName="_$(date +%F_%H-%M-%S)";
folderName="$TEST_NAME$folderName";
folderPath="$RESULTS_FOLDER/$folderName";


mkdir -p $RESULTS_FOLDER;
mkdir $folderPath


#Do time estimate of program
{ time $COMMAND > program_output.txt 2> program_error_output.txt ; } 2> time_output.txt;

#Print the program output to screen
cat program_output.txt;

#Remove program output if specified
if [[ "$SAVE_PROGRAM_OUTPUT" == "no" ]];
then
    rm program_output.txt;
fi

#Running valgrind programs
if [[ "$USE_MASSIF" == "yes" ]];
then
    valgrind --tool=massif --time-unit=B --detailed-freq=100 --massif-out-file="$folderPath/massif_output.txt" --log-file="$folderPath/massif_log.txt" $COMMAND > /dev/null 2>&1;
fi

if [[ "$USE_CALLGRIND" == "yes" ]];
then
    valgrind --tool=callgrind --callgrind-out-file="$folderPath/callgrind_output.txt" --log-file="$folderPath/callgrind_log.txt" --cache-sim=yes --branch-sim=yes $COMMAND > /dev/null 2>&1;
fi 


#Move all outputs and logs to the appropriate folder
if [[ "$SAVE_PROGRAM_OUTPUT" != "no" ]];
then
    mv program_output.txt $folderPath/
fi

mv program_error_output.txt $folderPath/
mv time_output.txt $folderPath/


printf "Test name: $TEST_NAME\n"  >> $folderPath/test_arguments.txt;
printf "Results folder: $RESULTS_FOLDER\n"  >> $folderPath/test_arguments.txt;
printf "Command: $COMMAND\n"  >> $folderPath/test_arguments.txt;
printf "Use massif: $USE_MASSIF\n"  >> $folderPath/test_arguments.txt;
printf "Use callgrind: $USE_CALLGRIND\n" >> $folderPath/test_arguments.txt;
printf "Save program output: $SAVE_PROGRAM_OUTPUT\n" >> $folderPath/test_arguments.txt;

popd > /dev/null 2>&1;
