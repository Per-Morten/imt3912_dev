get_filename_component(TEST_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)
project(${TEST_NAME})
 

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE TEMPLATES "src/*.tpp")
file(GLOB_RECURSE INLINES "src/*.ipp")
file(GLOB_RECURSE HEADERS "src/*.h")

set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(${INCLUDE_DIRS})
add_executable (${TEST_NAME} ${SOURCES} ${TEMPLATES} ${INLINES}) 
target_link_libraries(${TEST_NAME} ${NOXSAMPLE_NOX_LIBRARY})
