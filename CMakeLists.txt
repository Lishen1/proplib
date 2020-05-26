cmake_minimum_required(VERSION 3.14)
project( propsdk )

option(EDITOR_ENABLED "Should the editor to be." YES)
option(EXAMPLES_ENABLED "Should the examples to be." YES)
option(TESTS_ENABLED "Should the tests to be." YES)
option(QT_EDITOR_ENABLED "Should the qt editor or imgui?." NO)

add_library( propsdk-cppfeatures INTERFACE )
target_compile_features( propsdk-cppfeatures INTERFACE cxx_std_17 )

include( cmake/warnings.cmake )
add_library( propsdk-cppwarnings INTERFACE )
set_project_warinigs( propsdk-cppwarnings )

target_include_directories( propsdk-cppfeatures INTERFACE include )

include( cmake/dependency.cmake )


add_library( propsdk INTERFACE )

set_dependency ( propsdk-cppfeatures INTERFACE)

target_link_libraries( propsdk INTERFACE propsdk-cppfeatures propsdk-cppwarnings)

message( STATUS "Enabled examples: ${EXAMPLES_ENABLED}" )
message( STATUS "Enabled    tests: ${TESTS_ENABLED}" )

add_subdirectory(src)

if ( EXAMPLES_ENABLED )
    add_subdirectory(examples)
endif()

if ( TESTS_ENABLED )
    add_subdirectory(tests)
endif()
