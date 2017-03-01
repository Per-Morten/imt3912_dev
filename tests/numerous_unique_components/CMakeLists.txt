get_filename_component(TEST_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)

add_definitions(-DTRIVIAL_COMPONENT_COUNT=200)
add_definitions(-DSEND_EVENT_PER_N_COMPONMENT=10)

project(${TEST_NAME})
 

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE TEMPLATES "src/*.tpp")
file(GLOB_RECURSE INLINES "src/*.ipp")
file(GLOB_RECURSE HEADERS "src/*.h")


set(ALL_SOURCES
    ${SOURCES}
    ${TEMPLATES}
    ${INLINES}
    ${HEADERS})

set(INCLUDE_DIRS src/)


include_directories(${INCLUDE_DIRS})
add_executable (${TEST_NAME} ${ALL_SOURCES}) 
target_link_libraries(${TEST_NAME} ${NOXSAMPLE_NOX_LIBRARY} ${CMD_LIBRARY})
