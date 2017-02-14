get_filename_component(TEST_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)
project(${TEST_NAME})
 
include_directories(src)
file(GLOB SOURCES_CPP "src/*.cpp")
file(GLOB SOURCES_TPP "src/*.tpp")
file(GLOB SOURCES_IPP "src/*.ipp")
 
add_executable(${TEST_NAME} ${SOURCES_CPP} ${SOURCES_TPP} ${SOURCES_IPP})
target_link_libraries(${TEST_NAME} ${NOXSAMPLE_NOX_LIBRARY})
