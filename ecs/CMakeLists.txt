get_filename_component(ECS_PROJECT_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)
project(${ECS_PROJECT_NAME})
 

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE TEMPLATES "src/*.tpp")
file(GLOB_RECURSE INLINES "src/*.ipp")
file(GLOB_RECURSE HEADERS "src/*.h")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g3 -Og" ) #-Wall -pedantic -Wall -Wextra

set(INCLUDE_DIRS ${INCLUDE_DIRS} src/)

message(${INCLUDE_DIRS})
include_directories(${INCLUDE_DIRS})
add_executable (${ECS_PROJECT_NAME} ${SOURCES} ${TEMPLATES} ${INLINES}) 
target_link_libraries(${ECS_PROJECT_NAME} ${NOXSAMPLE_NOX_LIBRARY})
