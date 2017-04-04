set(NOX_GTEST_LIBRARY gtest_main)
set(NOX_ECS_LIBRARY ecs_library)
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
add_library(${NOX_ECS_LIBRARY} STATIC ${ALL_SOURCES})
target_link_libraries(${NOX_ECS_LIBRARY} ${NOXSAMPLE_NOX_LIBRARY})


# CREATE ECS MAIN
set(ECS_MAIN ecs)

project(${ECS_MAIN})

include_directories(${INCLUDE_DIRS})
add_executable(${ECS_MAIN} src/tests/main.cpp)
target_link_libraries(${ECS_MAIN} ${NOX_ECS_LIBRARY})

# CREATE TROND_TEST
file(GLOB_RECURSE SOURCES "src/trondtest/*.cpp")
file(GLOB_RECURSE TEMPLATES "src/trondtest/*.tpp")
file(GLOB_RECURSE INLINES "src/trondtest/*.ipp")
file(GLOB_RECURSE HEADERS "src/trondtest/*.h")

set(ALL_SOURCES
    ${SOURCES}
    ${TEMPLATES}
    ${INLINES})

set(TROND_TEST trondtest)

project(${TROND_TEST})

include_directories(${INCLUDE_DIRS})
add_executable(${TROND_TEST} ${ALL_SOURCES})
target_link_libraries(${TROND_TEST} ${NOX_ECS_LIBRARY})

# CREATE GOOGLE TESTS
# add_google_test(smart_handle_test src/tests/SmartHandle.cpp)
