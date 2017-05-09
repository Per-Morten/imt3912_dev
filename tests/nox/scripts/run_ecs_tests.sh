#!/bin/bash

function readableNumber 
{
    result="$1";
    while [ $(echo $result | wc -m) -lt $2 ];
    do
        result="0$result";
    done
}

compileVariations=14;
function compile
{
    case $1 in
        0)
        ARG="$2"
        PREFIX="no_optimization_toggles"
        shift # past argument=value
        ;;
        1)
        ARG="-DNOX_ECS_COMPONENT_VIRTUAL=TRUE $2"
        PREFIX="ecs_component_virtual"
        shift # past argument=value
        ;;
        2)
        ARG="-DNOX_ECS_COMPONENT_UNIQUE_PTR_VIRTUAL=TRUE $2"
        PREFIX="ecs_component_unique_ptr_virtual"
        shift # past argument=value
        ;;
        3)
        ARG="-DNOX_USE_STRING_TYPE_ID=TRUE $2"
        PREFIX="use_string_type_id"
        shift # past argument=value
        ;;
        4)
        ARG="-DNOX_ENTITYMANAGER_USE_LOCKED_QUEUE=TRUE $2"
        PREFIX="entitymanager_use_locked_queue"
        shift # past argument=value
        ;;
        5)
        ARG="-DNOX_ENTITYMANAGER_POOL_USE_LOCK_FREE_STACK=TRUE $2"
        PREFIX="entitymanager_pool_use_lock_free_stack"
        shift # past argument=value
        ;;
        6)
        ARG="-DNOX_ECS_LAYERED_EXECUTION_UPDATE=TRUE $2"
        PREFIX="ecs_layered_execution_update"
        shift # past argument=value
        ;;
        7)
        ARG="-DNOX_ECS_LAYERED_EXECUTION_ENTITY_EVENTS=TRUE $2"
        PREFIX="ecs_layered_execution_entity_events"
        shift # past argument=value
        ;;
        8)
        ARG="-DNOX_ECS_LAYERED_EXECUTION_LOGIC_EVENTS=TRUE $2"
        PREFIX="ecs_layered_execution_logic_events"
        shift # past argument=value
        ;;
        9)
        ARG="-DNOX_EVENT_USE_LINEAR_ALLOCATOR=TRUE $2"
        PREFIX="event_use_linear_allocator"
        shift # past argument=value
        ;;
        10)
        ARG="-DNOX_EVENT_USE_HEAP_ALLOCATOR=TRUE $2"
        PREFIX="event_use_heap_allocator"
        shift # past argument=value
        ;;
        11)
        ARG="-DNOX_ATOMIC_USE_SEQ_CST=TRUE $2"
        PREFIX="atomic_use_seq_cst"
        shift # past argument=value
        ;;
        12)
        ARG="-DNOX_LOCKFREESTACK_USE_LINEAR_ALLOCATOR=TRUE $2"
        PREFIX="lockfreestack_use_linear_allocator"
        shift # past argument=value
        ;;
        13)
        ARG="-DNOX_LOCKFREESTACK_USE_HEAP_ALLOCATOR=TRUE $2"
        PREFIX="lockfreestack_use_heap_allocator"
        shift # past argument=value
        ;;
    esac

    pushd $root > /dev/null 2>&1;
    ./compile.sh --make-arguments="-j4" --cmake-arguments=$ARG;
    popd > /dev/null 2>&1;
}

root="../../.."
resultsRoot="../results/ecs";
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

j=0;
while [ $j -lt $compileVariations ];
do
    compile $j;

    i="256";
    count="0";
    while [ $count -lt 8 ];
    do
        readableNumber $i 5;
        nr=$result;
    
        ./run_test.sh --test-name="${compiler}_${nr}" --results-folder="$resultsRoot/$PREFIX/memory_usage/sprite_actors" --command="./build/bin/ecs_memory_usage_sprite -actor_amount $i --logrm warning" --save-program-output=yes;
        ((i=i*2));
        ((count++));
    done

    ((j++));
done

j=0;
while [ $j -lt $compileVariations ];
do
    compile $j;

    i="256";
    count="0";
    while [ $count -lt 8 ];
    do
        readableNumber $i 5;
        nr=$result;
    
        ./run_test.sh --test-name="${compiler}_${nr}" --results-folder="$resultsRoot/$PREFIX/memory_usage/transform_actors" --command="./build/bin/ecs_memory_usage_transform -actor_amount $i --logrm warning" --save-program-output=yes;
        ((i=i*2));
        ((count++));
    done

    ((j++));
done

#j=0;
#while [ $j -lt $compileVariations ];
#do
#    compile $j;
#
#    i="256";
#    count="0";
#    while [ $count -lt 8 ];
#    do
#        readableNumber $i 5;
#        nr=$result;
#    
#        ./run_test.sh --test-name="${compiler}_deletes_01_actors_${nr}" --results-folder="$resultsRoot/$PREFIX/fast_spawning" --command="./build/bin/ecs_fast_spawning -actor_amount $i -deletion_amount 1 --logrm warning" --save-program-output=yes;
#        ./run_test.sh --test-name="${compiler}_deletes_10_actors_${nr}" --results-folder="$resultsRoot/$PREFIX/fast_spawning" --command="./build/bin/ecs_fast_spawning -actor_amount $i -deletion_amount 10 --logrm warning" --save-program-output=yes;
#    
#        ((i=i*2));
#        ((count++));
#    done
#    ((j++));
#done
#
#k=0;
#while [ $k -lt $compileVariations ];
#do
#    componentCount="0";
#    j="2";
#    while [ $componentCount -lt 3 ];
#    do
#        compile $k "-DNUC_TRIVIAL_COMPONENT_COUNT=${j}";
#    
#        i=1;
#        count=0;
#        while [ $count -lt 8 ];
#        do
#            readableNumber $i 4;
#            nr=$result;
#        
#            ./run_test.sh --test-name="${compiler}_components_${j}_actors_${nr}" --results-folder="$resultsRoot/$PREFIX/numerous_unique_components" --command="./build/bin/ecs_numerous_unique_components -actor_amount $i --logrm warning" --save-program-output=yes;
#            
#        
#            ((i=i*2));
#            ((count++));
#        done
#    
#        ((j=j*10));
#        ((componentCount++));
#    done
#
#    ((k++));
#done
#
#
#### ENDING TESTS WITH GCC. STARTING TESTS WITH CLANG ###
#
#
#
#compiler="clang";
#resultsRoot="../results/ecs";
#
#pushd $root > /dev/null 2>&1;
#rm -r build;
#./compile.sh --cmake-arguments="-DUSE_CLANG=ON" --make-arguments="-j4";
#
#
##Getting various hardware information
#hardware_info_root="$resultsRoot/hardware_info/$(date +%F_%H-%M-%S)":
#mkdir -p $hardware_info_root;
#
##hardware info
#sudo lshw -short >> $hardware_info_root/hardware_info_short.txt;
#
##hardware info verbose
#sudo lshw >> $hardware_info_root/hardware_info_long.txt;
#
##cpu info
#sudo lscpu >> $hardware_info_root/cpu_info.txt;
#
##battery info
#upower -d >> $hardware_info_root/battery_info.txt
#
##Copying cmake cache
#cp build/CMakeCache.txt $hardware_info_root/cmakecache_copy.txt;
#
#popd > /dev/null 2>&1;
#
#
#j=0;
#while [ $j -lt $compileVariations ];
#do
#    compile $j;
#
#    i="256";
#    count="0";
#    while [ $count -lt 8 ];
#    do
#        readableNumber $i 5;
#        nr=$result;
#    
#        ./run_test.sh --test-name="${compiler}_${nr}" --results-folder="$resultsRoot/$PREFIX/memory_usage/sprite_actors" --command="./build/bin/ecs_memory_usage_sprite -actor_amount $i --logrm warning" --save-program-output=yes;
#        ((i=i*2));
#        ((count++));
#    done
#
#    ((j++));
#done
#
#j=0;
#while [ $j -lt $compileVariations ];
#do
#    compile $j;
#
#    i="256";
#    count="0";
#    while [ $count -lt 8 ];
#    do
#        readableNumber $i 5;
#        nr=$result;
#    
#        ./run_test.sh --test-name="${compiler}_${nr}" --results-folder="$resultsRoot/$PREFIX/memory_usage/transform_actors" --command="./build/bin/ecs_memory_usage_transform -actor_amount $i --logrm warning" --save-program-output=yes;
#        ((i=i*2));
#        ((count++));
#    done
#
#    ((j++));
#done
#
#j=0;
#while [ $j -lt $compileVariations ];
#do
#    compile $j;
#
#    i="256";
#    count="0";
#    while [ $count -lt 8 ];
#    do
#        readableNumber $i 5;
#        nr=$result;
#    
#        ./run_test.sh --test-name="${compiler}_deletes_01_actors_${nr}" --results-folder="$resultsRoot/$PREFIX/fast_spawning" --command="./build/bin/ecs_fast_spawning -actor_amount $i -deletion_amount 1 --logrm warning" --save-program-output=yes;
#        ./run_test.sh --test-name="${compiler}_deletes_10_actors_${nr}" --results-folder="$resultsRoot/$PREFIX/fast_spawning" --command="./build/bin/ecs_fast_spawning -actor_amount $i -deletion_amount 10 --logrm warning" --save-program-output=yes;
#    
#        ((i=i*2));
#        ((count++));
#    done
#    ((j++));
#done
#
#k=0;
#while [ $k -lt $compileVariations ];
#do
#    componentCount="0";
#    j="2";
#    while [ $componentCount -lt 3 ];
#    do
#        compile $k "-DNUC_TRIVIAL_COMPONENT_COUNT=${j}";
#    
#        i=1;
#        count=0;
#        while [ $count -lt 8 ];
#        do
#            readableNumber $i 4;
#            nr=$result;
#        
#            ./run_test.sh --test-name="${compiler}_components_${j}_actors_${nr}" --results-folder="$resultsRoot/$PREFIX/numerous_unique_components" --command="./build/bin/ecs_numerous_unique_components -actor_amount $i --logrm warning" --save-program-output=yes;
#            
#        
#            ((i=i*2));
#            ((count++));
#        done
#    
#        ((j=j*10));
#        ((componentCount++));
#    done
#
#    ((k++));
#done
