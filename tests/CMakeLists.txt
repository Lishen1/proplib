project(tests)

include ( ${CMAKE_SOURCE_DIR}/cmake/CPM.cmake )

CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  VERSION 1.10.x
  OPTIONS
      "INSTALL_GTEST OFF"
      "gtest_force_shared_crt"
)

add_executable(serdeser_test serdeser/main.cpp)

target_link_libraries(serdeser_test propsdk gtest gtest_main gmock)
add_test(NAME SerealzationDeserialization COMMAND serdeser_test)

enable_testing() 



