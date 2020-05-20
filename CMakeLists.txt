cmake_minimum_required(VERSION 3.14)
project( propsdk )

option(EXAMPLES_ENABLED "Should the examples to be." YES)
option(TESTS_ENABLED "Should the tests to be." YES)

add_library( cppfeatures INTERFACE )
target_compile_features( cppfeatures INTERFACE cxx_std_17 )

include( cmake/warnings.cmake )
add_library( cppwarnings INTERFACE )
set_project_warinigs( cppwarnings )

target_include_directories( cppfeatures INTERFACE include )

include ( cmake/CPM.cmake )

CPMAddPackage(
  NAME easyloggingpp
  GITHUB_REPOSITORY amrayn/easyloggingpp
  VERSION 9.96.7
  OPTIONS "build_static_lib ON"
)

CPMAddPackage(
  NAME yaml-cpp
  GITHUB_REPOSITORY jbeder/yaml-cpp
  # 0.6.2 uses deprecated CMake syntax
  VERSION 0.6.3
  # 0.6.3 is not released yet, so use a recent commit
  GIT_TAG 012269756149ae99745b6dafefd415843d7420bb 
  OPTIONS
    "YAML_CPP_BUILD_TESTS Off"
    "YAML_CPP_BUILD_CONTRIB Off"
    "YAML_CPP_BUILD_TOOLS Off"
)

add_library( propsdk INTERFACE )

# easyloggingpp headers bug manually add
target_include_directories( propsdk INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/_deps/easyloggingpp-src/src )


target_link_libraries( propsdk INTERFACE cppfeatures cppwarnings easyloggingpp yaml-cpp)

message( STATUS "Enabled examples: ${EXAMPLES_ENABLED}" )
message( STATUS "Enabled    tests: ${TESTS_ENABLED}" )

if ( EXAMPLES_ENABLED )
    add_subdirectory(examples)
endif()

if ( TESTS_ENABLED )
    add_subdirectory(tests)
endif()
