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
        actorNames="$actorName\n$actorNames"
        
        let j=j+1
    done

    let i=i+2
done

#If shuffle is enabled, randomize the order of the actor names
if [ $mode = "shuffle" ];
then
    actorNames="$(printf "$actorNames" | shuf)"
fi

#Prints out the world
printf "{\n"
printf "\t\"actors\":\n"
printf "\t[\n"

for actorName in $actorNames; 
do
    printf "\t\t{\n"
    printf "\t\t\t\"extend\": \"$actorName\"\n"
    printf "\t\t}\n"
done

printf "\t]\n"
printf "}\n"

