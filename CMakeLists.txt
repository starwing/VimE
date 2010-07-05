# VimE - the Vim Extensible
# See docs/CMake.html for instructions about how to buid VimE with
# CMake.

# VimE DO NOT support in-source build.
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed.
	    enter build directory and exec \"cmake ..\"")
endif()


# set project name.
project(VimE)
cmake_minimum_required(VERSION 2.6)


# set project informations.
set(PACKAGE_NAME VimE)
set(PACKAGE_VERSION 0.1Alpha)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "weasley.wx@gmail.com")


# set base locations.
set(VIME_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(VIME_BUILDED_INCLUDE_DIR "${VIME_BINARY_DIR}/include")
set(VIME_EXAMPLES_BINARY_DIR ${VIME_BINARY_DIR}/examples)
set(VIME_MAIN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(VIME_MAIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(VIME_TOOLS_BINARY_DIR ${VIME_BINARY_DIR}/bin)


# Add path for custom modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${VIME_MAIN_SRC_DIR}/utils/cmake"
    "${VIME_MAIN_SRC_DIR}/utils/cmake/modules"
    )


# all options and checks are in AddVimEDefinitions module.
include(AddVimEDefinitions)


# set platform
if (WIN32)
    if (CYGWIN)
	set(VIME_ON_WIN32 0)
	set(VIME_ON_UNIX 1)
    else(CYGWIN)
	set(VIME_ON_WIN32 1)
	set(VIME_ON_UNIX 0)
    endif(CYGWIN)
    set(LTDL_SHLIB_EXT ".dll")
    set(EXECUTE_EXT ".exe")

    # Maximum path length is 160 for non-unicode paths
    set(MAXPATHLEN 160)
else(WIN32)
    if (UNIX)
	set(VIME_ON_WIN32 0)
	set(VIME_ON_UNIX 1)
	
	if (APPLE)
	    set(LTDL_SHLIB_EXT ".dylib")
	else(APPLE)
	    set(LTDL_SHLIB_EXT ".so")
	endif(APPLE)
	set(EXECUTE_EXT "")

	# FIXME: Maximum path length is currently set to 'safe' fixed value
	set(MAXPATHLEN 2024)
    else(UNIX)
	MESSAGE(SEND_ERROR "Unable to determine platform")
    endif(UNIX)
endif(WIN32)


# inlcude config file. all config variables are prefix of ENABLE_, and
# should written into configure files (config.h and defs.h).
include(config-ix)


# set configure files
configure_file(
    ${VIME_MAIN_INCLUDE_DIR}/config.h.in
    ${VIME_BUILDED_INCLUDE_DIR}/config.h
    )

configure_file(
    ${VIME_MAIN_INCLUDE_DIR}/defs.h.in
    ${VIME_BUILDED_INCLUDE_DIR}/defs.h
    )


# notice that if a variable is begining with ENABLE_ means it will
# export to configure file. and if a variable is begining with VIME_
# it means it will not export. and if a variable is begining with
# VIME_BUILD_ it only interaction with the command line compiler
# flags.


option(VIME_BUILD_PIC "Build Position-Independent Code" ON)


set(VIME_BUILD_PIC 0)
if (VIME_BUILD_PIC)
    if (XCODE)
	# Xcode has -mdynamic-no-pic on by default, which overrides -fPIC. I don't
	# know how to disable this, so just force VIME_BUILD_PIC off for now.
	message(STATUS "Warning: -fPIC not supported with Xcode.")
    else(XCODE)
	if (SUPPORTS_FPIC_FLAG)
	    message(STATUS "Building with -fPIC")
	    add_VIME_definitions(-fPIC)
	    set(VIME_BUILD_PIC 1)
	elseif (NOT WIN32)
	    message(STATUS "Warning: -fPIC not supported.")
	endif()
    endif()
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VIME_TOOLS_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VIME_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VIME_BINARY_DIR}/lib)


# set(CMAKE_VERBOSE_MAKEFILE true)

add_vime_definitions(-D__STDC_LIMIT_MACROS)
add_vime_definitions(-D__STDC_CONSTANT_MACROS)


# MSVC has a gazillion warnings with this.
if (MSVC)
  option(VIME_BUILD_WARNINGS "Enable compiler warnings." OFF)
else(MSVC)
  option(VIME_BUILD_WARNINGS "Enable compiler warnings." ON)
endif()


# option for compiler warnings.
option(VIME_BUILD_PEDANTIC "Compile with pedantic enabled." ON)
option(VIME_BUILD_WERROR "Fail and stop if a warning is triggered." OFF)


# Build 64 version of VimE.
if (CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT WIN32)
    # TODO: support other platforms and toolchains.
    option(VIME_BUILD_32_BITS "Build 32 bits executables and libraries." OFF)
    if (VIME_BUILD_32_BITS)
	message(STATUS "Building 32 bits executables and libraries.")
	add_vime_definitions(-m32)
	list(APPEND CMAKE_EXE_LINKER_FLAGS -m32)
	list(APPEND CMAKE_SHARED_LINKER_FLAGS -m32)
    endif(VIME_BUILD_32_BITS)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT WIN32)

 
# command arguments for MS Visual C.
if (MSVC)
    # List of valid CRTs for MSVC
    set(MSVC_CRT
	MD
	MDd)

    set(VIME_USE_CRT "" CACHE STRING "Specify VC++ CRT to use for debug/release configurations.")
    add_vime_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
    add_vime_definitions(-D_SCL_SECURE_NO_WARNINGS -DCRT_NONSTDC_NO_WARNINGS)
    add_vime_definitions(-D_SCL_SECURE_NO_DEPRECATE)
    #add_vime_definitions(-wd4146 -wd4503 -wd4996 -wd4800 -wd4244 -wd4624)
    #add_vime_definitions(-wd4355 -wd4715 -wd4180 -wd4345 -wd4224)

    # Suppress 'new behavior: elements of array 'array' will be default initialized'
    #add_vime_definitions(-wd4351)

    if (NOT ${VIME_USE_CRT} STREQUAL "")
        list(FIND MSVC_CRT ${VIME_USE_CRT} idx)
        if (idx LESS 0)
            message(FATAL_ERROR "Invalid value for VIME_USE_CRT: ${VIME_USE_CRT}.
Valid options are one of: ${MSVC_CRT}")
        endif(idx LESS 0)
        add_vime_definitions("/${VIME_USE_CRT}")
        message(STATUS "Using VC++ CRT: ${VIME_USE_CRT}")
    endif(NOT ${VIME_USE_CRT} STREQUAL "")

    # Enable warnings
    if (VIME_BUILD_WARNINGS)
        add_vime_definitions(/W4 /Wall)
        if (VIME_BUILD_PEDANTIC)
            # No MSVC equivalent available
        endif (VIME_BUILD_PEDANTIC)
    endif (VIME_BUILD_WARNINGS)
    if (VIME_BUILD_WERROR)
        add_vime_definitions(/WX)
    endif (VIME_BUILD_WERROR)
elseif (CMAKE_COMPILER_IS_GNUC)
    if (VIME_BUILD_WARNINGS)
        add_vime_definitions(-Wall -W -Wno-unused-parameter -Wwrite-strings)
        if (VIME_BUILD_PEDANTIC)
            add_vime_definitions(-pedantic -Wno-long-long)
        endif(VIME_BUILD_PEDANTIC)
    endif(VIME_BUILD_WARNINGS)
    if (VIME_BUILD_WERROR)
        add_vime_definitions(-Werror)
    endif(VIME_BUILD_WERROR)
endif(MSVC)

include_directories(${VIME_BINARY_DIR}/include ${VIME_MAIN_INCLUDE_DIR})


# load toolkit functions used to add install files.
include(AddVimE)


# add subdirectory contains sources
add_subdirectory(lib/Support)
add_subdirectory(lib/StaticData)
add_subdirectory(lib/Core)
add_subdirectory(lib/System)
add_subdirectory(lib/UI)


# build tools programs
option(VIME_BUILD_TOOLS "Build VimE tool programs." ON)
add_subdirectory(tools)

# build example programs
# option(VIME_BUILD_EXAMPLES "Build VimE example programs." OFF)
# add_subdirectory(examples)


# install include header files
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.inc"
    )


install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.inc"
    )

# vim: ft=cmake ts=8 sw=4 sts=4 ai et nu sta:
