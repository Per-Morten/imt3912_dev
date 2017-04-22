set(NOX_GTEST_LIBRARY gtest_main)
set(INCLUDE_DIRS src/)

function(add_google_test target)

    project(${target})

    add_executable(${target} ${ARGN})

    include_directories(${INCLUDE_DIRS})

    target_link_libraries(${target}
                          ${NOX_ECS_LIBRARY}
                          ${NOX_GTEST_LIBRARY}
                          ${NOX_LINK_TARGET})

    add_test(NAME ${target}
             COMMAND $<TARGET_FILE:${target}>
             WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endfunction()

function(create_test_case TARGET)
    file(GLOB_RECURSE SOURCES src/tests/${TARGET}/*.cpp)
    file(GLOB_RECURSE TEMPLATES src/tests/${TARGET}/*.tpp)
    file(GLOB_RECURSE INLINES src/tests/${TARGET}/*.ipp)
    file(GLOB_RECURSE HEADERS src/tests/${TARGET}/*.h)
    
    set(ALL_SOURCES
        ${SOURCES}
        ${TEMPLATES}
        ${INLINES})
    
    SET(NAME ecs_${TARGET})

    project(${NAME})
    
    include_directories(${INCLUDE_DIRS} src/tests/${TARGET})
    add_executable(${NAME} ${ALL_SOURCES})
    target_link_libraries(${NAME} ${NOX_ECS_LIBRARY} ${CMD_LIBRARY})
endfunction()

project(${NOX_ECS_LIBRARY})

file(GLOB_RECURSE SOURCES "src/nox/*.cpp")
file(GLOB_RECURSE TEMPLATES "src/nox/*.tpp")
file(GLOB_RECURSE INLINES "src/nox/*.ipp")
file(GLOB_RECURSE HEADERS "src/nox/*.h")

set(ALL_SOURCES
    ${SOURCES}
    ${TEMPLATES}
    ${INLINES}
    ${HEADERS})

# Debug flags: "${CMAKE_CXX_FLAGS} -O0 -g3 -Og -ggdb3"
# Regular flags: "${CMAKE_CXX_FLAGS} -O3 -Wall"
# Think of adding -Wextra.
set(NOXSAMPLE_CXX_FLAGS ${NOXSAMPLE_CXX_FLAGS} "-O3 -Wall -S")

# CREATE ECS LIBRARY
include_directories(${INCLUDE_DIRS})

# For EventArgument Allocator:
#add_definitions(-DNOX_EVENT_USE_LINEAR_ALLOCATOR)
#add_definitions(-DNOX_EVENT_USE_HEAP_ALLOCATOR)
add_definitions(-DNOX_EVENT_USE_LOCK_FREE_ALLOCATOR)

# For EntityManager Queues in general:
#add_definitions(-DNOX_ENTITYMANAGER_USE_LOCKED_QUEUE)
add_definitions(-DNOX_ENTITYMANAGER_USE_LOCK_FREE_STACK)

# For EntityManager Pool:
#add_definitions(-DNOX_ENTITYMANAGER_POOL_USE_LOCKED_FREE_STACK)
add_definitions(-DNOX_ENTITYMANAGER_POOL_USE_LOCKED_QUEUE)

# For LockFreeStack Allocator:
#add_definitions(-DNOX_LOCKFREESTACK_USE_LINEAR_ALLOCATOR)
#add_definitions(-DNOX_LOCKFREESTACK_USE_HEAP_ALLOCATOR)
add_definitions(-DNOX_LOCKFREESTACK_USE_LOCK_FREE_ALLOCATOR)

# For Layered Execution:
add_definitions(-DNOX_ECS_LAYERED_EXECUTION_UPDATE)
add_definitions(-DNOX_ECS_LAYERED_EXECUTION_ENTITY_EVENTS)
add_definitions(-DNOX_ECS_LAYERED_EXECUTION_LOGIC_EVENTS)

# For Atomics:
#add_definitions(-DNOX_ATOMIC_USE_SEQ_CST)
add_definitions(-DNOX_ATOMIC_USE_RELAXED)

# For TypeIdentifiers:
#add_definitions(-DNOX_USE_STRING_TYPE_ID)
add_definitions(-DNOX_USE_NUMERIC_TYPE_ID)

# For Virtual And UniquePtr:
#add_definitions(-DNOX_ECS_COMPONENT_UNIQUE_PTR_VIRTUAL)
#add_definitions(-DNOX_ECS_COMPONENT_VIRTUAL)
add_definitions(-DNOX_ECS_COMPONENT_NON_VIRTUAL)

add_library(${NOX_ECS_LIBRARY} STATIC ${ALL_SOURCES})
target_link_libraries(${NOX_ECS_LIBRARY} ${NOXSAMPLE_NOX_LIBRARY})



# CREATE ECS MAIN
set(ECS_MAIN ecs)

project(${ECS_MAIN})

include_directories(${INCLUDE_DIRS})
add_executable(${ECS_MAIN} src/tests/main.cpp)
target_link_libraries(${ECS_MAIN} ${NOX_ECS_LIBRARY})

add_definitions(-DTRIVIAL_COMPONENT_COUNT=${NUC_TRIVIAL_COMPONENT_COUNT})
add_definitions(-DSEND_LOCAL_EVENT_PER_N_COMPONMENT=13)
add_definitions(-DSEND_GLOBAL_EVENT_PER_N_COMPONMENT=11)

create_test_case(console_template)
create_test_case(window_template)
create_test_case(memory_usage)
create_test_case(fast_spawning)
create_test_case(numerous_unique_components)

# CREATE GOOGLE TESTS
# add_google_test(smart_handle_test src/tests/SmartHandle.cpp)
