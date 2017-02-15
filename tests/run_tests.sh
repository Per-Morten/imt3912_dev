#!/bin/bash

pushd .. > /dev/null 2>&1


printf "Running all tests\n\n"


printf "Memory Usage - Empty Actors:\n"
printf "Running with 100 actors\n"
./build/bin/memory_usage-empty_actors -actors_amount 100 --logrm warning
printf "\n"

printf "Memory Usage - Empty Actors\n"
printf "Running with 1000 actors\n"
./build/bin/memory_usage-empty_actors -actors_amount 1000 --logrm warning
printf "\n"

printf "Memory Usage - Empty Actors\n"
printf "Running with 10000 actors\n"
./build/bin/memory_usage-empty_actors -actors_amount 10000 --logrm warning
printf "\n"




popd > /dev/null 2>&1
