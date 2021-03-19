cmake_minimum_required(VERSION 3.14)

set(PROPLIB_NAME propsdk)
project( ${PROPLIB_NAME} )

set(PROPLIB_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(PROPLIB_BINARY_DIR ${PROJECT_BINARY_DIR})
set(PROPLIB_OUTPUT_DIR
    "${PROPLIB_BINARY_DIR}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
    
option(BUILD_SHARED_LIBS            "The shared library" OFF)
option(BUILD_PROPLIB_EXAMPLE        "Enable examples" ON)
option(BUILD_PROPLIB_TEST           "Enable tests" ON)
option(BUILD_PROPLIB_WITH_EDITOR    "Enable gui editor(default: imgui)" ON)
option(BUILD_PROPLIB_WITH_QT_EDITOR "Should the qt editor or imgui (default: imgui)" OFF)

option(BUILD_PROPLIB_CLANG_TIDY_AUTO     "Enable auto clang-tidy" OFF)
option(BUILD_PROPLIB_CLANG_TIDY_FIX      "Enable autofix clang-tidy" OFF)

if(MSVC)
  if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  endif()
  if(UNIX)
    message(STATUS "Use position independent code")
    set_property(GLOBAL PROPERTY POSITION_INDEPENDENT_CODE ON)
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE STRING "Build configuration 'Release' or 'Debug'." FORCE)
endif()

# cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(Utils)
include(clang-tidy)
include(depends)

add_subdirectory(src)

# example
if(BUILD_PROPLIB_EXAMPLE)
  add_subdirectory(example)
endif()

# test
if(BUILD_PROPLIB_TEST)
  add_subdirectory(test)
endif()

log(TEXT
    "Target name                                    ${PROPLIB_NAME}"
    "Target config                              --> ${CMAKE_BUILD_TYPE}"
    "Build shared library                       --> ${BUILD_SHARED_LIBS}"
    "Build feature_traker examples                  ${BUILD_PROPLIB_EXAMPLE}"
    "Build feature_traker test                      ${BUILD_PROPLIB_TEST}"
    "Build with editor:                             ${BUILD_PROPLIB_WITH_EDITOR}"
    " - qt editor                                   ${BUILD_PROPLIB_WITH_QT_EDITOR}"
    "Auto clang-tidy                                ${BUILD_PROPLIB_CLANG_TIDY_AUTO}"
    "Auto clang-format                              ${BUILD_PROPLIB_CLANG_TIDY_FIX}"
    SUMMARIZE)