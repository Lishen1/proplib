get_property(CLANG_TIDY_INITIALIZED GLOBAL "" PROPERTY CLANG_TIDY_INITIALIZED SET)
if (CLANG_TIDY_INITIALIZED)
  return()
endif()

set_property(GLOBAL PROPERTY CLANG_TIDY_INITIALIZED true)

find_program(CLANG_TIDY clang-tidy)
if(NOT CLANG_TIDY)
  message(STATUS "Did not find clang-tidy, target tidy is disabled.")

  function(clang_tidy)
    set(optionValueArgs CLANG_TIDY_AUTO CLANG_TIDY_FIX )
    set(oneValueArgs TARGET )
    set(multiValueArgs INCLUDE_DIRECTORIES SOURCES)
    cmake_parse_arguments(CT "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  endfunction()
else()
  message(STATUS "Found clang-tidy, use \"make tidy\" to run it.")
  set(CLANG_TIDY_FOUND
      true
      CACHE BOOL "Found clang-tidy.")

  if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") # Matches "Clang" and "AppleClang"
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
      message(STATUS "Please enable readability-redundant-member-init (disabled due to #32966)")
    endif()
  endif()

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  function(clang_tidy)
    set(optionValueArgs CLANG_TIDY_AUTO CLANG_TIDY_FIX )
    set(oneValueArgs TARGET )
    set(multiValueArgs INCLUDE_DIRECTORIES SOURCES)
    cmake_parse_arguments(CT "${optionValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(EXISTS ${PROJECT_BINARY_DIR}/compile_commands.json)
      list(APPEND CLANG_TIDY_FLAGS -p ${PROJECT_BINARY_DIR}/compile_commands.json)
    endif()
    
    if(CT_CLANG_TIDY_FIX)
      list(APPEND CLANG_TIDY_FLAGS -fix)
    endif()
    
    foreach(src ${CT_SOURCES})
      list(APPEND CLANG_TIDY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${src} )
    endforeach(src)
    
    # get target include directories
    get_target_property(LINK_TARGETS ${CT_TARGET} LINK_LIBRARIES)

    foreach(target ${LINK_TARGETS})

      if( TARGET ${target} )
        get_target_property(target_type_${target} ${target} TYPE)
        get_target_property(target_aliased ${target} ALIASED_TARGET)
        if(TARGET ${target_aliased})
          set(target ${target_aliased})
        endif()
        get_target_property(interface_dirs ${target} INTERFACE_INCLUDE_DIRECTORIES)
        if( NOT ${target_type_${target}} STREQUAL "INTERFACE_LIBRARY")
          get_target_property(dirs ${target} INCLUDE_DIRECTORIES)
        endif()
        
        if(interface_dirs)
          list(APPEND CT_INCLUDE_DIRECTORIES ${interface_dirs})
        endif()
        if(dirs)
          list(APPEND CT_INCLUDE_DIRECTORIES ${dirs})
        endif()
      endif()
    endforeach(target)
    
    list(REMOVE_DUPLICATES CT_INCLUDE_DIRECTORIES)
    
    foreach(src ${CT_INCLUDE_DIRECTORIES})
      string(REGEX MATCH "(<INSTALL_INTERFACE)" SKIP_INTERFACE ${src})
      if( NOT SKIP_INTERFACE)
        # message(STATUS "${src}")
        list(APPEND CLANG_TIDY_PUBLIC_HEADER_PREFIX -I ${src})
      endif()
    endforeach(src)
    
    # enable c++17 by default
    set(CLANG_TIDY_COMPILE_FLAGS -- ${CLANG_TIDY_PUBLIC_HEADER_PREFIX} -std=c++17)
    
    add_custom_target(
      tidy_${CT_TARGET}
      COMMAND ${CLANG_TIDY} ${CLANG_TIDY_FLAGS} ${CLANG_TIDY_SOURCES} 
              ${CLANG_TIDY_COMPILE_FLAGS}
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      COMMAND_EXPAND_LISTS
      )
      
    if(CT_CLANG_TIDY_AUTO)
      add_dependencies(${CT_TARGET} tidy_${CT_TARGET})
    endif()
  endfunction()
endif()

mark_as_advanced(CLANG_TIDY_FOUND)
