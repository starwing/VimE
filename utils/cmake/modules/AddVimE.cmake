# TODO: implement this macros

function(vime_check_source_file_list) # {{{1
    # check the *.c source file in the source directory contains in
    # file list.
    set(listded ${ARGN})
    file(GLOB globbed_file *.c)
    foreach (g ${globbed_file})
        get_filename_component(fn ${g} NAME)
        list(FIND listded ${fn} idx)
        if (idx LESS 0)
          message(SEND_ERROR "Found unknown source file ${g}
Please update ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt\n")
        endif()
    endforeach()
endfunction(vime_check_source_file_list)

function(vime_process_source OUT_VAR) # {{{1
    set(source ${ARGN})
    vime_check_source_file_list(${source})
    if (MSVC_IDE)
        file(GLOB hds *.h)
        if (hds)
            set_source_files_properties(${hds} PROPERTIES HEADER_FILE_ONLY ON)
            list(APPEND source ${hds})
        endif()
    endif()
    set(${OUT_VAR} ${source} PARENT_SCOPE)
endfunction(vime_process_source)


function(get_system_libs OUT_VAR) # {{{1
    # Returns in `return_var' a list of system libraries used by LLVM.
    if (NOT MSVC)
        if(MINGW)
            set(system_libs ${system_libs} imagehlp psapi)
        elseif (CMAKE_HOST_UNIX)
            if (HAVE_LIBDL)
                set(system_libs ${system_libs} ${CMAKE_DL_LIBS})
            endif()
            if (LLVM_ENABLE_THREADS AND HAVE_LIBPTHREAD)
                set(system_libs ${system_libs} pthread)
            endif()
        endif(MINGW)
    endif(NOT MSVC)
    set(${return_var} ${system_libs} PARENT_SCOPE)
endfunction(get_system_libs)


macro(add_vime_library name) # {{{1
    vime_process_source(ALL_FILES ${ARGN})
    add_library(${name} ${ALL_FILES})
    set(vime_libs ${vime_libs} ${name} PARENT_SCOPE)
    set(vime_lib_targets ${vime_lib_targets} ${name} PARENT_SCOPE)
    if (VIME_COMMON_DEPENDS)
        add_dependencies(${name} ${VIME_COMMON_DEPENDS})
    endif()

    install(TARGETS ${name}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endmacro(add_vime_library name)

macro(add_vime_loadable_module name) # {{{1
    if (NOT VIME_ON_UNIX)
        message(STATUS "Loadable modules not supported on this platform.
${name} ignored.")
    else()
        vime_process_source(ALL_FILES ${ARGN})
        add_library(${name} MODULE ${ALL_FILES})
        set_target_properties(${name} PROPERTIES PREFIX "")

        if (APPLE)
            # Darwin-specific linker flags for loadable modules.
            set_target_properties(${name} PROPERTIES
                LINK_FLAGS "-Wl,-flat_namespace -Wl,-undefined -Wl,suppress")
        endif()

        install(TARGETS ${name}
            LIBRARY DESTINATION lib${LLVM_LIBDIR_SUFFIX}
            ARCHIVE DESTINATION lib${LLVM_LIBDIR_SUFFIX})
    endif()
endmacro(add_vime_loadable_module name)

macro(add_vime_executable name) # {{{1
    vime_process_source(ALL_FILES ${ARGN})
    if (EXCLUDE_FROM_ALL)
        add_executable(${name} EXCLUDE_FROM_ALL ${ALL_FILES})
    else()
        add_executable(${name} ${ALL_FILES})
    endif()
    set(EXCLUDE_FROM_ALL OFF)
    if (VIME_USED_LIBS)
        foreach (lib ${VIME_USED_LIBS})
            target_link_libraries(${name} ${lib})
        endforeach(lib)
    endif(VIME_USED_LIBS)
    get_system_libs(vime_system_libs)
    if (vime_system_libs)
        target_link_libraries(${name} ${vime_system_libs})
    endif()
    if (VIME_COMMON_DEPENDS)
        add_dependencies(${name} ${VIME_COMMON_DEPENDS})
    endif(VIME_COMMON_DEPENDS)
endmacro(add_vime_executable name)

macro(add_vime_tool name) # {{{1
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VIME_TOOLS_BINARY_DIR})
    if (NOT VIME_BUILD_TOOLS)
        set(EXCLUDE_FROM_ALL ON)
    endif()
    add_vime_executable(${name} ${ARGN})
    if (VIME_BUILD_TOOLS)
        install(TARGETS ${name} RUNTIME DESTINATION bin)
    endif()
endmacro(add_vime_tool name)

macro(add_vime_example name) # {{{1
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLVM_EXAMPLES_BINARY_DIR})
    if (NOT VIME_BUILD_TOOLS)
        set(EXCLUDE_FROM_ALL ON)
    endif()
    add_vime_executable(${name} ${ARGN})
    if (VIME_BUILD_TOOLS)
        install(TARGETS ${name} RUNTIME DESTINATION bin)
    endif()
endmacro(add_vime_example name)

# }}}1
# vim: ft=cmake fdm=marker fdc=2 ts=8 sw=4 sts=4 ai et nu sta:
