cmake_minimum_required(VERSION 3.14)
project( propsdk )

option(EDITOR_ENABLED "Should the editor to be." YES)
option(EXAMPLES_ENABLED "Should the examples to be." YES)
option(TESTS_ENABLED "Should the tests to be." YES)

add_library( propsdk-cppfeatures INTERFACE )
target_compile_features( propsdk-cppfeatures INTERFACE cxx_std_17 )

include( cmake/warnings.cmake )
add_library( propsdk-cppwarnings INTERFACE )
set_project_warinigs( propsdk-cppwarnings )

target_include_directories( propsdk-cppfeatures INTERFACE include )

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

CPMAddPackage(
  NAME QtnProperty
  GITHUB_REPOSITORY ueberaccelerate/QtnProperty
  GIT_TAG master
  OPTIONS
  #  "YAML_CPP_BUILD_TESTS Off"
  #  "YAML_CPP_BUILD_CONTRIB Off"
  #  "YAML_CPP_BUILD_TOOLS Off"
)

CPMAddPackage(
  NAME httplib
  GITHUB_REPOSITORY yhirose/cpp-httplib
  GIT_TAG master
)


add_library( propsdk INTERFACE )

# easyloggingpp headers bug manually add
target_include_directories( propsdk INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/_deps/easyloggingpp-src/src )


target_link_libraries( propsdk INTERFACE propsdk-cppfeatures propsdk-cppwarnings easyloggingpp yaml-cpp QtnProperty httplib)

message( STATUS "Enabled examples: ${EXAMPLES_ENABLED}" )
message( STATUS "Enabled    tests: ${TESTS_ENABLED}" )

add_subdirectory(src)

if ( EXAMPLES_ENABLED )
    add_subdirectory(examples)
endif()

if ( TESTS_ENABLED )
    add_subdirectory(tests)
endif()
