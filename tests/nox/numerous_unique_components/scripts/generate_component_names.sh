#!/bin/bash

numberOfComponents=$1
componentName=$2
i=0

while [ $i -lt $numberOfComponents ];
do
    printf "$componentName" 
    printf "$i\n"
    ((i++))
done
