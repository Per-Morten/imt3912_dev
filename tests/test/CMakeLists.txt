get_filename_component(SAMPLE_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)

project(${SAMPLE_NAME})

set(SOURCES
	src/main.cpp
)

add_executable(${SAMPLE_NAME} ${SOURCES})
target_link_libraries(${SAMPLE_NAME} ${NOXSAMPLE_NOX_LIBRARY})
