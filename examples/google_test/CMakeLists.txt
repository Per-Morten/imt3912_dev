#Need to define this in order to find the google test library
set(NOX_GTEST_LIBRARY gtest_main)

#Function for adding tests more easily, should be placed in the root folder of unit tests
function(add_google_test target)

    project(${target})

    add_executable(${target} ${ARGN})
    
    target_link_libraries(${target} 
                          ${NOXSAMPLE_NOX_LIBRARY}
                          ${NOX_GTEST_LIBRARY}
                          ${NOX_LINK_TARGET})

    add_test(NAME ${target}
             COMMAND $<TARGET_FILE:${target}>
             WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endfunction()

#Creation of each test
add_google_test(sample_1
                sample1.h
                sample1.cpp
                sample1_unittest.cpp)

#Possible to write it like this as well
set(SOURCE
    sample2.h
    sample2.cpp
    sample2_unittest.cpp)

add_google_test(sample_2 ${SOURCE})


add_google_test(sample_3
                sample3.h
                sample3_unittest.cpp)
