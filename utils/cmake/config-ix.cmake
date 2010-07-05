include(CheckIncludeFile)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckCSourceCompiles)

if (UNIX)
    # Used by check_symbol_exists:
    set(CMAKE_REQUIRED_LIBRARIES m)
endif()

# Helper macros and functions {{{1

macro(add_c_include result files)
    set(${result} "")
    foreach (file_name ${files})
	set(${result} "${${result}}#include <${file_name}>\n")
    endforeach()
endmacro(add_c_include)

function(check_type_exists type files variable)
    add_c_include(includes "${files}")
    CHECK_C_SOURCE_COMPILES("${includes}
${type} typeVar;
int main(void) {
    return 0;
}
" ${variable})
endfunction(check_type_exists)

macro(set_headers name)
    set(${name} "")
    foreach (h ${ARGN})
        string(REGEX REPLACE "[.\\/]" "_" hf ${h})
        string(TOUPPER ${hf} hf)
        if (HAVE_${hf})
            set(${name} ${${name}} "${h}")
        endif()
    endforeach()
endmacro(set_headers)

macro(add_vime_option name docstring big normal)
    if (uc_VIME_FEATURES_TYPE STREQUAL "FULL")
        option(${name} ${docstring} ON)
    elseif (uc_VIME_FEATURES_TYPE STREQUAL "BIG")
        option(${name} ${docstring} ${big})
    elseif (uc_VIME_FEATURES_TYPE STREQUAL "NORMAL")
        option(${name} ${docstring} ${normal})
    elseif (uc_VIME_FEATURES_TYPE STREQUAL "MINIAL")
        option(${name} ${docstring} OFF)
    endif()
endmacro(add_vime_option)

macro(add_build_option name docstring debug release)
    if (uc_VIME_BUILD_TYPE STREQUAL "RELEASE")
        option(${name} ${docstring} ${release})
    else()
        option(${name} ${docstring} ${debug})
    endif()
endmacro(add_build_option)


# build options {{{1
# set build type
if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: [Release Debug]." FORCE)
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" uc_CMAKE_BUILD_TYPE)
message(STATUS "Use ${uc_CMAKE_BUILD_TYPE} Build Type")

# enable assertions in VimE
add_build_option(ENABLE_ASSERTIONS "Enable assertions" OFF ON)
add_build_option(ENABLE_INLINE "Enable function inline" ON OFF)


# add debug flags use ENABLE_ASSERTIONS
if (ENABLE_ASSERTIONS)
    # MSVC doesn't like _DEBUG on release builds. See PR 4379.
    if (NOT MSVC)
	add_definitions(-D_DEBUG)
    endif()
elseif (NOT uppercase_CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    add_definitions(-DNDEBUG)
endif()


# feature settings {{{1

# set feature options.
set(VIME_FEATURES_TYPE BIG CACHE STRING "the featrues built into VimE, options are [FULL BIG NORMAL MINIAL].")
string(TOUPPER "${VIME_FEATURES_TYPE}" uc_VIME_FEATURES_TYPE)
message(STATUS "Use ${uc_VIME_FEATURES_TYPE} feature type.")


add_vime_option(ENABLE_GUI "Enable GUI Support." ON OFF)
add_vime_option(ENABLE_MULTILANG "Enable Multi-Language Support." ON ON)
add_vime_option(ENABLE_ICONV "Enable IConv Support." ON ON)


# include checks {{{1
check_include_file(assert.h HAVE_ASSERT_H)
check_include_file(stddef.h HAVE_STDDEF_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdio.h HAVE_STDIO_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(string.h HAVE_STRING_H)


if (VIME_ON_WIN32)
    check_include_file(windows.h HAVE_WINDOWS_H)
    if (NOT HAVE_WINDOWS_H)
	message(FATAL_ERROR "Can't find windows.h in Win32, Stop.")
    endif()

elseif (VIME_ON_UNIX)
    # TODO: check GTK for GUI settings
    # set(ENABLE_GUI OFF)
endif()


# library checks {{{1
if (VIME_ON_WIN32)
elseif (VIME_ON_UNIX)
    check_library_exists(pthread pthread_create "" HAVE_LIBPTHREAD)
    check_library_exists(pthread pthread_getspecific "" HAVE_PTHREAD_GETSPECIFIC)
    check_library_exists(pthread pthread_rwlock_init "" HAVE_PTHREAD_RWLOCK_INIT)
    check_library_exists(dl dlopen "" HAVE_LIBDL)
endif()


# function checks {{{1

if (VIME_ON_WIN32)
elseif (VIME_ON_UNIX)
    check_symbol_exists(getpagesize unistd.h HAVE_GETPAGESIZE)
    check_symbol_exists(getrusage sys/resource.h HAVE_GETRUSAGE)
    check_symbol_exists(setrlimit sys/resource.h HAVE_SETRLIMIT)
    check_function_exists(isatty HAVE_ISATTY)

    check_symbol_exists(__GLIBC__ stdio.h VIME_USING_GLIBC)
    if (VIME_USING_GLIBC)
	add_llvm_definitions(-D_GNU_SOURCE)
    endif()
endif()


# set types {{{1

set_headers(headers "sys/types.h" "inttypes.h" "stdint.h")
check_type_exists(uint32_t "${headers}" HAVE_UINT32_T)
check_type_exists(u_int32_t "${headers}" HAVE_U_INT32_T)
check_type_exists(uint64_t "${headers}" HAVE_UINT64_T)
check_type_exists(u_int64_t "${headers}" HAVE_U_INT64_T)

set_headers(headers "stdio.h")
check_type_exists(size_t "${headers}" HAVE_SIZE_T)
check_type_exists(ssize_t "${headers}" HAVE_SSIZE_T)


# available programs checks {{{1
function(vime_find_program name)
    string(TOUPPER ${name} NAME)
    find_program(VIME_PATH_${NAME} ${name})
    mark_as_advanced(VIME_PATH_${NAME})
    if(VIME_PATH_${NAME})
	set(HAVE_${NAME} 1 CACHE INTERNAL "Is ${name} available ?")
	mark_as_advanced(HAVE_${NAME})
    else(VIME_PATH_${NAME})
	set(HAVE_${NAME} "" CACHE INTERNAL "Is ${name} available ?")
    endif(VIME_PATH_${NAME})
endfunction()


# vime_find_program(bison)



# check PIC flags {{{1
include(CheckCCompilerFlag)

# On windows all code is position-independent and mingw warns if -fPIC
# is in the command-line.
if (NOT WIN32)
    check_c_compiler_flag("-fPIC" SUPPORTS_FPIC_FLAG)
endif()


# set base types {{{1

if (MSVC)
  set(error_t int)
  set(mode_t "unsigned short")
  set(LTDL_SHLIBPATH_VAR "PATH")
  set(LTDL_SYSSEARCHPATH "")
  set(LTDL_DLOPEN_DEPLIBS 1)
  set(SHLIBEXT ".lib")
  set(LTDL_OBJDIR "_libs")
  set(HAVE_STRTOLL 1)
  set(strtoll "_strtoi64")
  set(strtoull "_strtoui64")
  set(stricmp "_stricmp")
  set(strdup "_strdup")
else (MSVC)
  set(LTDL_SHLIBPATH_VAR "LD_LIBRARY_PATH")
  set(LTDL_SYSSEARCHPATH "") # TODO
  set(LTDL_DLOPEN_DEPLIBS 0)  # TODO
endif(MSVC)


# FIXME: Signal handler return type, currently hardcoded to 'void'
set(RETSIGTYPE void)


# Threads settings {{{1
if (ENABLE_THREADS)
  if (HAVE_PTHREAD_H OR WIN32)
    set(ENABLE_THREADS 1)
  endif()
endif()

if (ENABLE_THREADS)
  message(STATUS "Threads enabled.")
else(ENABLE_THREADS)
  message(STATUS "Threads disabled.")
endif()

# vim: ft=cmake fdm=marker fdc=2 ts=8 sw=4 sts=4 ai et nu sta:
