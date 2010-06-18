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

macro(add_c_include result files) # {{{2
    set(${result} "")
    foreach (file_name ${files})
	set(${result} "${${result}}#include <${file_name}>\n")
    endforeach()
endmacro(add_c_include)

function(check_type_exists type files variable) # {{{2
    add_c_include(includes "${files}")
    CHECK_C_SOURCE_COMPILES("${includes}
${type} typeVar;
int main(void) {
    return 0;
}
" ${variable})
endfunction(check_type_exists)

macro(set_headers name) # {{{2
    set(${name} "")
    foreach (h ${ARGN})
        string(REGEX REPLACE "[.\\/]" "_" hf ${h})
        string(TOUPPER ${hf} hf)
        if (HAVE_${hf})
            set(${name} ${${name}} "${h}")
        endif()
    endforeach()
endmacro(set_headers)

function(add_options type name docstring) # {{{2
    set(opt OFF)
    set(i 3)
    while (i LESS ${ARGC})
        if (ARGV${i} STREQUAL uc_${type})
            math(EXPR i "${i} + 1")
            set(opt ${ARGV${i}})
        elseif (ARGV${i} STREQUAL "DEFAULT"
                OR ARGV${i} STREQUAL "REQUIRE")
            break()
        endif()
        math(EXPR i "${i} + 1")
    endwhile()
        
    if (ARGV${i} STREQUAL "DEFAULT")
        math(EXPR i "${i} + 1")
        set(opt ${ARGV${i}})
        math(EXPR i "${i} + 1")
    endif()

    if (ARGV${i} STREQUAL "REQUIRE")
        math(EXPR i "${i} + 1")
        foreach (x RANGE ${i} ${ARGC})
            set(cond ${cond} ${ARGV${x}})
        endforeach(x)
        if (NOT (${cond}))
            set(opt OFF)
        endif(NOT (${cond}))
    endif()
    message("     ${name} \t:\t${opt}")
    option(${name} ${docstring} ${opt})
endfunction(add_options)

macro(add_feature_option) # {{{2
    add_options(VIME_FEATURES_TYPE ${ARGN})
endmacro()

macro(add_build_option) # {{{2
    add_options(CMAKE_BUILD_TYPE ${ARGN})
endmacro()
# }}}2

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


# types checks {{{1

set_headers(headers "sys/types.h" "inttypes.h" "stdint.h")
check_type_exists(uint32_t "${headers}" HAVE_UINT32_T)
check_type_exists(u_int32_t "${headers}" HAVE_U_INT32_T)
check_type_exists(uint64_t "${headers}" HAVE_UINT64_T)
check_type_exists(u_int64_t "${headers}" HAVE_U_INT64_T)

set_headers(headers "stdio.h")
check_type_exists(size_t "${headers}" HAVE_SIZE_T)
check_type_exists(ssize_t "${headers}" HAVE_SSIZE_T)


# inline checks {{{1

CHECK_C_SOURCE_COMPILES("
inline int func(int x) { return x + 2; }

int main(void) {
    func(10);
    return 0;
}
" HAVE_INLINE)


# PIC flags checks {{{1
include(CheckCCompilerFlag)

# On windows all code is position-independent and mingw warns if -fPIC
# is in the command-line.
if (NOT WIN32)
    check_c_compiler_flag("-fPIC" SUPPORTS_FPIC_FLAG)
endif()


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

# build options settings {{{1
# set build type
if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: [Release Debug]." FORCE)
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" uc_CMAKE_BUILD_TYPE)
message(STATUS "Use ${uc_CMAKE_BUILD_TYPE} Build Type")

# enable assertions in VimE
add_build_option(ENABLE_ASSERTIONS "Enable assertions"
    RELEASE OFF BUILD ON REQUIRE HAVE_ASSERT_H)

# enable inline support in VimE.
add_build_option(ENABLE_INLINE "Enable function inline"
    RELEASE ON BUILD OFF REQUIRE HAVE_INLINE)


# add debug flags use ENABLE_ASSERTIONS
if (ENABLE_ASSERTIONS)
    # MSVC doesn't like _DEBUG on release builds. See PR 4379.
    if (NOT MSVC)
	add_vime_definitions(-D_DEBUG)
    endif()
elseif (NOT uc_CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    add_vime_definitions(-O3 -DNDEBUG)
endif()


# feature settings {{{1

# set feature options.
set(VIME_FEATURES_TYPE BIG CACHE STRING "the featrues built into VimE, options are [FULL BIG NORMAL MINIAL].")
string(TOUPPER "${VIME_FEATURES_TYPE}" uc_VIME_FEATURES_TYPE)
message(STATUS "Use ${uc_VIME_FEATURES_TYPE} feature type.")


add_feature_option(ENABLE_GUI "Enable GUI Support."
    FULL ON BIG ON NORMAL OFF MINIAL OFF
    REQUIRE GTK2_FOUND OR WIN32)
add_feature_option(ENABLE_INTL "Enable Multi-Language Support."
    FULL ON BIG ON NORMAL ON MINIAL OFF
    REQUIRE HAVE_LIBINTL_H OR WIN32)
add_feature_option(ENABLE_ICONV "Enable IConv Support."
    FULL ON BIG ON NORMAL OFF MINIAL OFF
    REQUIRE HAVE_ICONV_H OR WIN32)

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


# }}}1
# vim: ft=cmake fdm=marker fdc=2 ts=8 sw=4 sts=4 ai et nu sta:
