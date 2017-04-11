project(${CMD_LIBRARY})

set(INCLUDE_DIR src/)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE TEMPLATES "src/*.tpp")
file(GLOB_RECURSE INLINES "src/*.ipp")
file(GLOB_RECURSE HEADERS "src/*.h")

include_directories(${INCLUDE_DIR})

add_library(${CMD_LIBRARY} ${SOURCES} ${TEMPLATES} ${INLINES} ${HEADERS})

target_link_libraries(${CMD_LIBRARY} ${NOX_ECS_LIBRARY})
target_include_directories (${CMD_LIBRARY} PUBLIC ${INCLUDE_DIR})
