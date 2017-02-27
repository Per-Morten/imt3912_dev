valgrind_args="--suppressions=min.suppressions --leak-check=full --show-leak-kinds=reachable --read-var-info=yes --track-origins=yes --expensive-definedness-checks=yes --keep-stacktraces=alloc-and-free"

if [[ $# -lt 1 ]] 
    then
    valgrind ${valgrind_args} ./build/bin/ecs > run_log.txt 2> valgrind_log.txt
else
    if [[ $1 =~ "-P" ]] 
        then
        valgrind ${valgrind_args} ./build/bin/ecs 2> valgrind_log.txt
    elif [[ $1 =~ "-V" ]]
        then
        valgrind ${valgrind_args} ./build/bin/ecs > run_log.txt
    elif [[ $1 =~ "-A" ]]
        then
        valgrind ${valgrind_args} ./build/bin/ecs
    fi
fi

