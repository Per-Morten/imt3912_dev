#!/bin/bash

pushd ../.. > /dev/null 2>&1


printf "Running all tests\n\n"


printf "Memory Usage - Empty Actors:\n"
printf "Running with 100 actors\n"
./build/bin/empty_actors -actors_amount 100 --logrm warning
printf "\n"

printf "Running with 1000 actors\n"
./build/bin/empty_actors -actors_amount 1000 --logrm warning
printf "\n"

printf "Running with 10000 actors\n"
./build/bin/empty_actors -actors_amount 10000 --logrm warning
printf "\n\n"



printf "Memory Usage - Physics Actors:\n"
printf "Running with 100 actors\n"
./build/bin/physics_actors -actors_amount 100 --logrm warning
printf "\n"

printf "Running with 1000 actors\n"
./build/bin/physics_actors -actors_amount 1000 --logrm warning
printf "\n"

printf "Running with 10000 actors\n"
./build/bin/physics_actors -actors_amount 10000 --logrm warning
printf "\n\n"



printf "Memory Usage - Sprite Actors:\n"
printf "Running with 100 actors\n"
./build/bin/sprite_actors -actors_amount 100 --logrm warning
printf "\n"

printf "Running with 1000 actors\n"
./build/bin/sprite_actors -actors_amount 1000 --logrm warning
printf "\n"

printf "Running with 10000 actors\n"
./build/bin/sprite_actors -actors_amount 10000 --logrm warning
printf "\n\n"


printf "Fast Spawning - Empty Actors:\n"
./build/bin/fast_spawning -actor_amount 100 -world_path world/creator_world.json -deletion_amount 1 -actor_name EmptyActor --logrm warning
printf "\n"

popd > /dev/null 2>&1
