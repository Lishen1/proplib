cmake_minimum_required(VERSION 2.8)
project(http-client)

add_executable(http-client main.cpp)
target_link_libraries(http-client PRIVATE ${PROP_TARGET})