project(tests)

add_executable(serdeser_test serdeser/main.cpp)

target_link_libraries(serdeser_test propsdk gtest gtest_main gmock)
add_test(NAME SerealzationDeserialization COMMAND serdeser_test)

enable_testing() 



