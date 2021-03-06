#!/bin/bash

function readableNumber 
{
    result="$1";
    while [ $(echo $result | wc -m) -lt $2 ];
    do
        result="0$result";
    done
}


root="../../.."
resultsRoot="../results/nox";
compiler="gcc";

printf "Running all tests\n\n"

pushd $root > /dev/null 2>&1;
rm -r build;
./compile.sh --make-arguments="-j4";

#Getting various hardware information
hardware_info_root="$resultsRoot/hardware_info/$(date +%F_%H-%M-%S)":
mkdir -p $hardware_info_root;

#hardware info
sudo lshw -short >> $hardware_info_root/hardware_info_short.txt;

#hardware info verbose
sudo lshw >> $hardware_info_root/hardware_info_long.txt;

#cpu info
sudo lscpu >> $hardware_info_root/cpu_info.txt;

#battery info
upower -d >> $hardware_info_root/battery_info.txt

#Copying cmake cache
cp build/CMakeCache.txt $hardware_info_root/cmakecache_copy.txt;

popd > /dev/null 2>&1;


#i=0;
#while [ $i -lt 16 ];
#do
#    printf "\n### RUNNING COMPILATION TEST $i ###\n";
#    readableNumber $i 3;
#    nr=$result;  
#
#    pushd ../.. > /dev/null 2>&1;
#    printf "\n### REMOVING BUILD FOLDER IN COMPILATION TEST $i ###\n";
#    rm -r build;
#    printf "\n### RUN BASE COMPILATION IN COMPILATION TEST $i ###\n";
#    ./compile.sh --cmake-arguments="-DCOMPILATION_TEST=ON" --make-arguments="-j4";
#    popd > /dev/null 2>&1;
#    
#    printf "\n### RUNNING ACTUAL COMPILATION TEST $i ###\n";
#    cmakeArguments="\"-DECS_BUILD=OFF -DEXAMPLES_BUILD=OFF\"";
#    ./run_test.sh --test-name="$compiler_$nr" --results-folder="$root/compilation_test" --command="./compile.sh --cmake-arguments=$cmakeArguments" --use-massif=no --use-callgrind=no --save-program-output=yes;
#done


i="1";
count="0";
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="${compiler}_${nr}" --results-folder="$resultsRoot/memory_usage/empty_actors" --command="./build/bin/nox_empty_actors -actor_amount $i --logrm warning" --save-program-output=yes;
    ((i=i*2));
    ((count++));
done

i="1";
count="0";
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="${compiler}_${nr}" --results-folder="$resultsRoot/memory_usage/physics_actors" --command="./build/bin/nox_physics_actors -actor_amount $i --logrm warning" --save-program-output=yes;
    ((i=i*2));
    ((count++));
done

i="1";
count="0";
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="${compiler}_${nr}" --results-folder="$resultsRoot/memory_usage/sprite_actors" --command="./build/bin/nox_sprite_actors -actor_amount $i --logrm warning" --save-program-output=yes;
    ((i=i*2));
    ((count++));
done



i="1";
count="0";
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="${compiler}_deletes_01_empty_actors_${nr}" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name EmptyActor -actor_amount $i -deletion_amount 1 --logrm warning" --save-program-output=yes;
    ./run_test.sh --test-name="${compiler}_deletes_10_empty_actors_${nr}" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name EmptyActor -actor_amount $i -deletion_amount 10 --logrm warning" --save-program-output=yes;
    
    ./run_test.sh --test-name="${compiler}_deletes_01_sprite_actors_${nr}" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name SpriteActor -actor_amount $i -deletion_amount 1 --logrm warning" --save-program-output=yes;
    ./run_test.sh --test-name="${compiler}_deletes_10_sprite_actors_${nr}" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name SpriteActor -actor_amount $i -deletion_amount 10 --logrm warning" --save-program-output=yes;
    
    ./run_test.sh --test-name="${compiler}_deletes_01_physics_actors_${nr}" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name PhysicsActor -actor_amount $i -deletion_amount 1 --logrm warning" --save-program-output=yes;
    ./run_test.sh --test-name="${compiler}_deletes_10_physics_actors_${nr}" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name PhysicsActor -actor_amount $i -deletion_amount 10 --logrm warning" --save-program-output=yes;

    ((i=i*2));
    ((count++));
done

componentCount="0";
j="1";
while [ $componentCount -lt 8];
do
    pushd ../.. > /dev/null 2>&1;
    ./compile --cmake-arguments="-DNUC_TRIVIAL_COMPONENT_COMPONENT_COUNT=${j}" --make-arguments="-j4";
    popd > /dev/null 2>&1;

    i=1;
    count=0;
    while [ $count -lt 8 ];
    do
        readableNumber $i 4;
        nr=$result;
    
        ./run_test.sh --test-name="${compiler]_components_${j]_actors_${nr}" --results-folder="$resultsRoot/numerious_unique_components" --command="./build/bin/nox_numerious_unique_components -actor_amount $i --logrm warning" --save-program-output=yes;
        
    
        ((i=i*2));
        ((count++));
    done

    ((j=j*2));
    ((componentCount++));
done


resultsRoot="../results/ecs";


i="1";
count="0";
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="${compiler}_${nr}" --results-folder="$resultsRoot/memory_usage" --command="./build/bin/ecs_memory_usage -actor_amount $i --logrm warning" --save-program-output=yes;
    ((i=i*2));
    ((count++));
done



i="1";
count="0";
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="${compiler}_deletes_01_actors_${nr}" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/ecs_fast_spawning -actor_amount $i -deletion_amount 1 --logrm warning" --save-program-output=yes;
    ./run_test.sh --test-name="${compiler}_deletes_10_actors_${nr}" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/ecs_fast_spawning -actor_amount $i -deletion_amount 10 --logrm warning" --save-program-output=yes;

    ((i=i*2));
    ((count++));
done

componentCount="0";
j="1";
while [ $componentCount -lt 8];
do
    pushd ../.. > /dev/null 2>&1;
    ./compile --cmake-arguments="-DNUC_TRIVIAL_COMPONENT_COMPONENT_COUNT=${j}" --make-arguments="-j4";
    popd > /dev/null 2>&1;

    i=1;
    count=0;
    while [ $count -lt 8 ];
    do
        readableNumber $i 4;
        nr=$result;
    
        ./run_test.sh --test-name="${compiler]_components_${j]_actors_${nr}" --results-folder="$resultsRoot/numerious_unique_components" --command="./build/bin/ecs_numerious_unique_components -actor_amount $i --logrm warning" --save-program-output=yes;
        
    
        ((i=i*2));
        ((count++));
    done

    ((j=j*2));
    ((componentCount++));
done



### ENDING TESTS WITH GCC. STARTING TESTS WITH CLANG ###



compiler="clang";
resultsRoot="../results/nox";

pushd $root > /dev/null 2>&1;
rm -r build;
./compile.sh --cmake-arguments="-DUSE_CLANG=ON" --make-arguments="-j4";


#Getting various hardware information
hardware_info_root="$resultsRoot/hardware_info/$(date +%F_%H-%M-%S)":
mkdir -p $hardware_info_root;

#hardware info
sudo lshw -short >> $hardware_info_root/hardware_info_short.txt;

#hardware info verbose
sudo lshw >> $hardware_info_root/hardware_info_long.txt;

#cpu info
sudo lscpu >> $hardware_info_root/cpu_info.txt;

#battery info
upower -d >> $hardware_info_root/battery_info.txt

#Copying cmake cache
cp build/CMakeCache.txt $hardware_info_root/cmakecache_copy.txt;

popd > /dev/null 2>&1;


#i=0;
#while [ $i -lt 16 ];
#do
#    readableNumber $i 3;
#    nr=$result;  
#
#    pushd ../.. > /dev/null 2>&1;
#    rm -r build;
#    ./compile.sh --cmake-arguments="-DUSE_CLANG=ON -DCOMPILATION_TEST=ON" --make-arguments="-j4";
#    ./run_test.sh --test-name="$compiler_$nr" --results-folder="$root/compilation_test" --command="./compile.sh --cmake-arguments=\"-DUSE_CLANG=ON -DECS_BUILD=OFF -DEXAMPLES_BUILD=OFF\"" --use-massif=no --use-callgrind=no --save-program-output=yes;
#    popd > /dev/null 2>&1;
#
#done


i=1;
count=0;
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="$compiler_$nr" --results-folder="$resultsRoot/memory_usage/empty_actors" --command="./build/bin/nox_empty_actors -actor_amount $i --logrm warning" --save-program-output=yes;
    ((i=i*2));
    ((count++));
done

i=1;
count=0;
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="$compiler_$nr" --results-folder="$resultsRoot/memory_usage/physics_actors" --command="./build/bin/nox_physics_actors -actor_amount $i --logrm warning" --save-program-output=yes;
    ((i=i*2));
    ((count++));
done

i=1;
count=0;
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="$compiler_$nr" --results-folder="$resultsRoot/memory_usage/sprite_actors" --command="./build/bin/nox_sprite_actors -actor_amount $i --logrm warning" --save-program-output=yes;
    ((i=i*2));
    ((count++));
done



i=1;
count=0;
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="$compiler_deletes_01_empty_actors_$nr" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name EmptyActor -actor_amount $i -deletion_amount 1 --logrm warning" --save-program-output=yes;
    ./run_test.sh --test-name="$compiler_deletes_10_empty_actors_$nr" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name EmptyActor -actor_amount $i -deletion_amount 10 --logrm warning" --save-program-output=yes;
    
    ./run_test.sh --test-name="$compiler_deletes_01_sprite_actors_$nr" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name SpriteActor -actor_amount $i -deletion_amount 1 --logrm warning" --save-program-output=yes;
    ./run_test.sh --test-name="$compiler_deletes_10_sprite_actors_$nr" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name SpriteActor -actor_amount $i -deletion_amount 10 --logrm warning" --save-program-output=yes;
    
    ./run_test.sh --test-name="$compiler_deletes_01_physics_actors_$nr" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name PhysicsActor -actor_amount $i -deletion_amount 1 --logrm warning" --save-program-output=yes;
    ./run_test.sh --test-name="$compiler_deletes_10_physics_actors_$nr" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/nox_fast_spawning -actor_name PhysicsActor -actor_amount $i -deletion_amount 10 --logrm warning" --save-program-output=yes;

    ((i=i*2));
    ((count++));
done

componentCount=0;
j=1;
while [ $componentCount -lt 8];
do
    pushd ../.. > /dev/null 2>&1;
    ./compile --cmake-arguments="-DNUC_TRIVIAL_COMPONENT_COMPONENT_COUNT=$j" --make-arguments="-j4";
    popd > /dev/null 2>&1;

    i=1;
    count=0;
    while [ $count -lt 8 ];
    do
        readableNumber $i 4;
        nr=$result;
    
        ./run_test.sh --test-name="$compiler_components_$j_actors_$nr" --results-folder="$resultsRoot/numerious_unique_components" --command="./build/bin/nox_numerious_unique_components -actor_amount $i --logrm warning" --save-program-output=yes;
        
    
        ((i=i*2));
        ((count++));
    done

    ((j=j*2));
    ((componentCount++));
done


resultsRoot="../results/ecs";


i="1";
count="0";
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="${compiler}_${nr}" --results-folder="$resultsRoot/memory_usage" --command="./build/bin/ecs_memory_usage -actor_amount $i --logrm warning" --save-program-output=yes;
    ((i=i*2));
    ((count++));
done



i="1";
count="0";
while [ $count -lt 16 ];
do
    readableNumber $i 6;
    nr=$result;

    ./run_test.sh --test-name="${compiler}_deletes_01_actors_${nr}" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/ecs_fast_spawning -actor_amount $i -deletion_amount 1 --logrm warning" --save-program-output=yes;
    ./run_test.sh --test-name="${compiler}_deletes_10_actors_${nr}" --results-folder="$resultsRoot/fast_spawning" --command="./build/bin/ecs_fast_spawning -actor_amount $i -deletion_amount 10 --logrm warning" --save-program-output=yes;

    ((i=i*2));
    ((count++));
done

componentCount="0";
j="1";
while [ $componentCount -lt 8];
do
    pushd ../.. > /dev/null 2>&1;
    ./compile --cmake-arguments="-DNUC_TRIVIAL_COMPONENT_COMPONENT_COUNT=${j}" --make-arguments="-j4";
    popd > /dev/null 2>&1;

    i=1;
    count=0;
    while [ $count -lt 8 ];
    do
        readableNumber $i 4;
        nr=$result;
    
        ./run_test.sh --test-name="${compiler]_components_${j]_actors_${nr}" --results-folder="$resultsRoot/numerious_unique_components" --command="./build/bin/ecs_numerious_unique_components -actor_amount $i --logrm warning" --save-program-output=yes;
        
    
        ((i=i*2));
        ((count++));
    done

    ((j=j*2));
    ((componentCount++));
done

