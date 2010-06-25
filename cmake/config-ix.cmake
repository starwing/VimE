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
endmacro(add_c_include files result)

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
endmacro(add_vime_option name docstring big normal)


function(check_type_exists type files variable)
    add_c_include(includes "${files}")
    CHECK_C_SOURCE_COMPILES("${includes}
${type} typeVar;
int main(void) {
    return 0;
}
" ${variable})
endfunction()


# feature settings {{{1

# set feature options.
set(VIME_FEATURES_TYPE BIG CACHE STRING "the featrues built into VimE, options are [FULL BIG NORMAL MINIAL].")
string(TOUPPER "${VIME_FEATURES_TYPE}" uc_VIME_FEATURES_TYPE)
message(STATUS "Use ${uc_VIME_FEATURES_TYPE} feature type.")


add_vime_option(VIME_FEATURE_GUI "Enable GUI Support." ON OFF)
add_vime_option(VIME_FEATURE_MULTILANG "Enable Multi-Language Support." ON ON)
add_vime_option(VIME_FEATURE_ICONV "Enable IConv Support." ON ON)


# include checks {{{1
check_include_file(assert.h HAVE_ASSERT_H)


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


# set headers {{{1
set(headers "")
if (HAVE_SYS_TYPES_H)
    set(headers ${headers} "sys/types.h")
endif()

if (HAVE_INTTYPES_H)
    set(headers ${headers} "inttypes.h")
endif()

if (HAVE_STDINT_H)
    set(headers ${headers} "stdint.h")
endif()

check_type_exists(uint32_t "${headers}" HAVE_UINT32_T)
check_type_exists(u_int32_t "${headers}" HAVE_U_INT32_T)
check_type_exists(uint64_t "${headers}" HAVE_UINT64_T)
check_type_exists(u_int64_t "${headers}" HAVE_U_INT64_T)


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
if (VIME_ENABLE_THREADS)
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
