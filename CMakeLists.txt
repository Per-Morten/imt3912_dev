cmake_minimum_required(VERSION 2.8.11)

# Makes Eclipse C++ indexer find C++14 symbols (needs to be before project()!)
if(${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
	set(CMAKE_CXX_COMPILER_ARG1 "-std=c++14" CACHE STRING "C++ version for eclipse" FORCE)
endif()

project(nox-examples)

option(NOXSAMPLE_ENGINE_SUBPROJECT "Use the NOX Engine subproject rather than the system library" ON)
option(COMPILATION_TEST "If the current building is used for the compilation test or not" OFF)
option(EXAMPLES_BUILD "If the examples should be built or not" ON)

# Get the compile commands so that we can use youcompleteme.
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

if (NOXSAMPLE_ENGINE_SUBPROJECT)
	add_subdirectory(nox-engine)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
		message(WARNING "GCC versions below 4.8 might not be able to build because of missing C++11 support.")
	endif()

	include(CheckCXXCompilerFlag)

	set(HAS_CXX14 OFF)

	set(CXX14_FLAG -std=c++14)
	check_cxx_compiler_flag(${CXX14_FLAG} HAS_CXX14_FLAG)

	if (HAS_CXX14_FLAG)
		set(HAS_CXX14 ON)
	elseif (NOT HAS_CXX14)
		set(CXX14_FLAG -std=c++1y)
		check_cxx_compiler_flag(${CXX14_FLAG} HAS_CXX1Y_FLAG)
	endif ()

	if (HAS_CXX1Y_FLAG)
		set(HAS_CXX14 ON)
	elseif (NOT HAS_CXX14)
		set(CXX14_FLAG "-std=c++1y -stdlib=libc++")
		check_cxx_compiler_flag(${CXX14_FLAG} HAS_CXX1Y_STDLIB_FLAG)
	endif ()

	if (HAS_CXX1Y_STDLIB_FLAG)
		set(HAS_CXX14 ON)
	endif ()

	if (HAS_CXX14)
		set(NOXSAMPLE_CXX_FLAGS "${NOXSAMPLE_CXX_FLAGS} ${CXX14_FLAG}")
		message(STATUS "Setting c++ standard flag to ${CXX14_FLAG}")
	else ()
		message(FATAL_ERROR "Compiler does not support C++14 (${CXX14_FLAG})")
	endif ()

	check_cxx_compiler_flag(-fdiagnostics-color HAS_COLOR_OUTPUT)
	if (HAS_COLOR_OUTPUT)
		set(NOXSAMPLE_CXX_FLAGS "${NOXSAMPLE_CXX_FLAGS} -fdiagnostics-color=auto")
	endif ()

	check_cxx_compiler_flag(-flto HAS_LTO)
	check_cxx_compiler_flag(-Ofast HAS_OPTIMIZE_FAST)
	check_cxx_compiler_flag(-ffinite-math-only HAS_FINITE_MATH)
	check_cxx_compiler_flag(-s HAS_STRIP_SYMBOLS)

	if (HAS_LTO)
		message(STATUS "Enabling link-time optimizations (-flto)")
		set(NOXSAMPLE_CXX_FLAGS_RELEASE "${NOXSAMPLE_CXX_FLAGS_RELEASE} -flto")
		set(NOXSAMPLE_LINKER_FLAGS_RELEASE "${NOXSAMPLE_LINKER_FLAGS_RELEASE} -flto")

		if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
			# Need to use gcc-ar and gcc-ranlib for LTO with static libraries.
			# See http://gcc.gnu.org/gcc-4.9/changes.html
			set(CMAKE_AR gcc-ar)
			set(CMAKE_RANLIB gcc-ranlib)
		endif ()
	endif ()

	if (HAS_OPTIMIZE_FAST)
		message(STATUS "Enabling -Ofast optimization.")
		set(NOXSAMPLE_CXX_FLAGS_RELEASE "${NOXSAMPLE_CXX_FLAGS_RELEASE} -Ofast")
	endif ()

	if (HAS_FINITE_MATH)
		message(STATUS "Enabling -ffinite-math-only optimization.")
		set(NOXSAMPLE_CXX_FLAGS_RELEASE "${NOXSAMPLE_CXX_FLAGS_RELEASE} -ffinite-math-only")
	endif ()

	if (HAS_STRIP_SYMBOLS)
		message(STATUS "Stripping symbols (-s)")
		set(NOXSAMPLE_LINKER_FLAGS_RELEASE "${NOXSAMPLE_LINKER_FLAGS_RELEASE} -s")
	endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	if (NOT CMAKE_LIBRARY_ARCHITECTURE)
		if (CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(CMAKE_LIBRARY_ARCHITECTURE "x64")
		else ()
			set(CMAKE_LIBRARY_ARCHITECTURE "x86")
		endif ()
	endif ()

	set(NOXSAMPLE_CXX_FLAGS "${NOXSAMPLE_CXX_FLAGS} /MP")
else ()
	message(WARNING "You are using an unsupported compiler. Compilation has only been tested with GCC.")
endif ()

#Getting the gcc version
SET(EXECUTE_COMMAND gcc -dumpversion)
EXECUTE_PROCESS(COMMAND ${EXECUTE_COMMAND}
                OUTPUT_VARIABLE GCC_VERSION)

#Getting the Clang version
SET(EXECUTE_COMMAND clang -dumpversion)
EXECUTE_PROCESS(COMMAND ${EXECUTE_COMMAND}
                OUTPUT_VARIABLE CLANG_VERSION)

SET(CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID} CACHE STRING "Compiler id" FORCE)
SET(GCC_VERSION ${GCC_VERSION} CACHE STRING "GCC Version" FORCE)
SET(CLANG_VERSION ${CLANG_VERSION} CACHE STRING "Clang Version" FORCE)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib CACHE PATH "Single directory for all static libraries.")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib CACHE PATH "Single directory for all dynamic libraries on Unix.")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single directory for all executable and dynamic libraries on Windows.")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NOXSAMPLE_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${NOXSAMPLE_CXX_FLAGS_RELEASE}")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${NOXSAMPLE_LINKER_FLAGS_RELEASE}")

if (NOXSAMPLE_ENGINE_SUBPROJECT)
	if (NOX_BUILD_SHARED)
		set(NOXSAMPLE_NOX_LIBRARY nox-shared)
	elseif (NOX_BUILD_STATIC)
		set(NOXSAMPLE_NOX_LIBRARY nox-static)
	endif ()
else ()
	set(NOXSAMPLE_NOX_LIBRARY nox)
endif ()

if (NOT COMPILATION_TEST)
    enable_testing()
    
    add_subdirectory(tests)
    add_subdirectory(ecs)  

    if (EXAMPLES_BUILD)
        add_subdirectory(examples)
    endif ()
endif ()
