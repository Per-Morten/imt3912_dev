#!/bin/bash

mode="$1"

args=("$@")
argsSize=${#args[@]}
actorNames=""

#Goes through all the input arguments and puts the specified amount of each actor's name into actorNames
i=1
while [ $i -lt $((argsSize-1)) ];
do
    actorCount=${args[i]}
    actorName=${args[$((i+1))]}

    j=0
    while [ $j -lt $actorCount ];
    do
        actorNames="$actorName $actorNames"
        
        let j=j+1
    done

    let i=i+2
done

#If shuffle is enabled, randomize the order of the actor names
if [ $mode = "shuffle" ];
then
    actorNames="$(printf "$actorNames" | tr ' ' '\n' | shuf)"
fi

#Prints out the world
printf "{\n"
printf "\t\"actors\":\n"
printf "\t[\n"

first=true
for actorName in $actorNames; 
do
    if [ $first = false ];
    then
        printf ",\n"
    fi
    first=false

    printf "\t\t{\n"
    printf "\t\t\t\"extend\": \"$actorName\"\n"
    printf "\t\t}"
done

printf "\n"
printf "\t]\n"
printf "}\n"

