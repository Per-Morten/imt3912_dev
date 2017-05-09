#!/bin/bash

#Parses all arguments and put them into variables
for i in "$@"
do
    case $i in
        --grep-arguments=*)
        GREP_ARGUMENTS="${i#*=}"
        shift # past argument=value
        ;;
        --data-type=*)
        DATA_TYPE="${i#*=}"
        shift # past argument=value
        ;;
    esac
done

pushd ../../../../results > /dev/null 2>&1;


directories=$(find . -type d);

result_name=""
for grep_arg in $GREP_ARGUMENTS
do
    directories=$(printf "$directories" | grep $grep_arg)
    result_name="${result_name}_${grep_arg}";
done

result_name="${result_name}_$DATA_TYPE"
result_name=$(echo $result_name | cut -c 2-)

args_files=($(find $directories -type f | grep test_arguments))
time_files=($(find $directories -type f | grep time_output))
memory_files=($(find $directories -type f | grep massif_output))
cpu_files=($(find $directories -type f | grep program_output))


if [[ "$DATA_TYPE" == "time" ]];
then
    data=""
    i=0
    while [ $i -lt ${#args_files[@]} ];
    do
        data="${data}$(cat ${args_files[$i]} | grep Command | awk '{ for(i=1;i<=NF;i++) { if ($i ~ "actor_amount") { print $(i+1) } } }') "
        data="${data}$(cat ${time_files[$i]} | grep real | cut -f 2 | tr 'm' ' ' | tr -d 's' | awk '{ print $1 * 60 + $2 }')\n"
        ((i=i+1))
    done
elif [[ "$DATA_TYPE" == "cpu" ]];
then
    data=""
    i=0
    while [ $i -lt ${#args_files[@]} ];
    do
        data="${data}$(cat ${args_files[$i]} | grep Command | awk '{ for(i=1;i<=NF;i++) { if ($i ~ "actor_amount") { print $(i+1) } } }') "
        data="${data}$(cat ${cpu_files[$i]} | grep Cyclecount | awk '{print $2}')\n"

        ((i=i+1))
    done
elif [[ "$DATA_TYPE" == "memory" ]];
then
        data=""
    i=0
    while [ $i -lt ${#args_files[@]} ];
    do
        data="${data}$(cat ${args_files[$i]} | grep Command | awk '{ for(i=1;i<=NF;i++) { if ($i ~ "actor_amount") { print $(i+1) } } }') "
        data="${data}$(cat ${memory_files[$i]} | grep peak -B 3 | grep mem_heap_B | tr -d "mem_heap_B=")\n"

        ((i=i+1))
    done
else
    echo "Did not specify a valid data type!";
    pop > /dev/null 2>&1
    exit 1;
fi

printf "$data" | sort -t" " -k1 -n >> ${result_name}.csv

pop > /dev/null 2>&1
