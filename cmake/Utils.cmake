get_property(UTILS_INITIALIZED GLOBAL "" PROPERTY UTILS_INITIALIZED SET)
if (UTILS_INITIALIZED)
  return()
endif()

set_property(GLOBAL PROPERTY UTILS_INITIALIZED true)

include(GNUInstallDirs)

function(log)
  set(options SUMMARIZE)
  set(multiValueArgs TEXT)
  cmake_parse_arguments(FTLOG "${options}" "" "${multiValueArgs}" ${ARGN})
  if(FTLOG_SUMMARIZE)
    message(STATUS "################################################################")
  endif()

  foreach(src ${FTLOG_TEXT})
    message(STATUS "  ${src}.")
  endforeach(src)

  if(FTLOG_SUMMARIZE)
    message(STATUS "################################################################")
  endif()
endfunction()

function(add_target)
  set(optionsArgs EXECUTABLE EXPORTED)
  set(oneValueArgs TARGET PREFIX PUBLIC_PREFIX BINARY_DIRECTORY VERSION )
  set(multiValueArgs
      PUBLIC_HEADER
      PRIVATE_HEADER
      SOURCES
      PUBLIC_LINKS
      PRIVATE_LINKS)
  cmake_parse_arguments(FT "${optionsArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  
  if(NOT FT_PREFIX)
    set(FT_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
  


  foreach(src ${FT_PUBLIC_HEADER})
    list(APPEND FT_PUBLIC_HEADER_WITH_PREFIX ${FT_PUBLIC_PREFIX}/${src})
  endforeach(src)

  foreach(src ${FT_PRIVATE_HEADER})
    list(APPEND FT_PRIVATE_HEADER_WITH_PREFIX ${FT_PREFIX}/${src})
  endforeach(src)

  foreach(src ${FT_SOURCES})
    list(APPEND FT_SOURCE_WITH_PREFIX ${FT_PREFIX}/${src})
  endforeach(src)

  if(NOT FT_EXECUTABLE)
    add_library(
      ${FT_TARGET}
      ${FT_PRIVATE_HEADER_WITH_PREFIX}
      ${FT_PUBLIC_HEADER_WITH_PREFIX} ${FT_SOURCE_WITH_PREFIX})
  else()
    add_executable(
      ${FT_TARGET}
      ${FT_PRIVATE_HEADER_WITH_PREFIX} 
      ${FT_PUBLIC_HEADER_WITH_PREFIX} ${FT_SOURCE_WITH_PREFIX})
  endif()

  if(FT_BINARY_DIRECTORY)
    set_target_properties(${FT_TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${FT_BINARY_DIRECTORY}"
                                                  RUNTIME_OUTPUT_DIRECTORY "${FT_BINARY_DIRECTORY}")
  endif()
  if(FT_VERSION) 
    set_target_properties(${FT_TARGET} PROPERTIES VERSION ${FT_VERSION})
  endif()
  

  target_include_directories(${FT_TARGET} 
  PUBLIC 
    $<BUILD_INTERFACE:${FT_PUBLIC_PREFIX}>
    $<INSTALL_INTERFACE:include> 
  )

  target_link_libraries(
    ${FT_TARGET}
    PUBLIC ${FT_PUBLIC_LINKS}
    PRIVATE ${FT_PRIVATE_LINKS})

  if(FT_PUBLIC_PREFIX)
    source_group(
      TREE ${FT_PUBLIC_PREFIX}
      PREFIX "Header Files/Public"
      FILES ${FT_PUBLIC_HEADER_WITH_PREFIX})
    source_group(
      TREE ${FT_PUBLIC_PREFIX}
      PREFIX "Source Files/Public"
      FILES ${FT_PUBLIC_SOURCE_WITH_PREFIX})
  endif()
  if(FT_PREFIX)
    source_group(
      TREE ${FT_PREFIX}
      PREFIX "Header Files/Private"
      FILES ${FT_PRIVATE_HEADER_WITH_PREFIX})
    source_group(
      TREE ${FT_PREFIX}
      PREFIX "Source Files"
      FILES ${FT_PRIVATE_SOURCE_WITH_PREFIX})

    source_group(
      TREE ${FT_PREFIX}
      PREFIX "Source Files"
      FILES ${FT_SOURCE_WITH_PREFIX})
  endif()
  
  if( FT_EXPORTED )
    # install 
    install(
      TARGETS ${FT_TARGET}
      EXPORT ${PROJECT_NAME}-targets
      ARCHIVE
          DESTINATION ${CMAKE_INSTALL_LIBDIR}
      LIBRARY
          DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME
          DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
  endif()
  log(TEXT
      "Target           - ${FT_TARGET}"
      "Target Version   - ${FT_VERSION}"
      "Target Exported  - ${FT_EXPORTED}"
      "Export to target - ${PROJECT_NAME}Targets"
      "Prefix          - ${FT_PREFIX}"
      "Public Prefix   - ${FT_PUBLIC_PREFIX}"
      "* Public headers"
      "${FT_PUBLIC_HEADER}"
      "* Private headers"
      "${FT_PRIVATE_HEADER}"
      "* Sources"
      "${FT_SOURCES}"
      SUMMARIZE)
endfunction()
