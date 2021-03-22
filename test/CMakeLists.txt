option(BUILD_TESTING "Enabled testing" ON)

find_package(GTest REQUIRED)

add_target(
  TARGET
    serdeser_test
  SOURCES
    serdeser_main.cpp
  PRIVATE_LINKS
    propsdk
    GTest::GTest 
    GTest::Main
  BINARY_DIRECTORY
    ${PROPERTYSDK_OUTPUT_DIR}
  EXECUTABLE
)

add_target(
  TARGET
    http_test
  PREFIX
    ${CMAKE_CURRENT_SOURCE_DIR}/http-client
  SOURCES
    rang.hpp
    httplib.h
    main.cpp
  PRIVATE_LINKS
    propsdk
    GTest::GTest 
    GTest::Main
  BINARY_DIRECTORY
    ${PROPERTYSDK_OUTPUT_DIR}
  EXECUTABLE
)
#target_compile_definitions(http_test PRIVATE _WIN32_WINNT)

if(WIN32)
    target_link_libraries(http_test PRIVATE ws2_32)
endif()

add_test(SerealzationDeserialization serdeser_test)
add_test(HttpSerealzationDeserialization http_test)




