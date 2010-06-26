# TODO: implement this macros

# check the *.c source file in the source directory contains in file
# list.
function(vime_check_source_file_list)
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

function(vime_process_source OUT_VAR)
    set(source ${ARGN})
    vime_check_source_file_list(${source})
    if (MSVC_IDE)
        file(GLOB hds *.h)
        if (hds)
            set_source_files_properties(${hds} PROPERTIES HEADER_FILE_ONLY ON)
            list(APPEND source ${hds})
        endif()
    endif()
    set(${OUT_VAR} ${source} PARENT_SCORE)
endfunction(vime_process_source)


macro(add_vime_library name)
    vime_process_source(ALL_FILES ${ARGN})
    add_library(${name} ${ALL_FILES})
    set(vime_libs ${vime_libs} ${name} PARENT_SCORE)
    set(vime_lib_targets ${vime_lib_targets} ${name} PARENT_SCORE)
    if (VIME_COMMON_DEPENDS)
        add_dependencies(${name} ${VIME_COMMON_DEPENDS})
    endif()

    install(TARGETS ${name}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endmacro(add_vime_library name)

macro(add_vime_loadable_module name)
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

macro(add_vime_executable name)
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
    get_system_libs(llvm_system_libs)
    if( llvm_system_libs )
        target_link_libraries(${name} ${llvm_system_libs})
    endif()
    if( LLVM_COMMON_DEPENDS )
        add_dependencies( ${name} ${LLVM_COMMON_DEPENDS} )
    endif( LLVM_COMMON_DEPENDS )
endmacro(add_vime_executable name)

macro(add_vime_tool name)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VIME_TOOLS_BINARY_DIR})
    if (NOT VIME_BUILD_TOOLS)
        set(EXCLUDE_FROM_ALL ON)
    endif()
    add_vime_executable(${name} ${ARGN})
    if (VIME_BUILD_TOOLS)
        install(TARGETS ${name} RUNTIME DESTINATION bin)
    endif()
endmacro(add_vime_tool name)

macro(add_vime_example name)
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LLVM_EXAMPLES_BINARY_DIR})
    if (NOT VIME_BUILD_TOOLS)
        set(EXCLUDE_FROM_ALL ON)
    endif()
    add_vime_executable(${name} ${ARGN})
    if (VIME_BUILD_TOOLS)
        install(TARGETS ${name} RUNTIME DESTINATION bin)
    endif()
endmacro(add_vime_example name)


# vim: ft=cmake ts=8 sw=4 sts=4 ai et nu sta:
