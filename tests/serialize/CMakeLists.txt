cmake_minimum_required(VERSION 2.8)
project(serialize)

add_executable(serialize main.cpp)
target_link_libraries(serialize PRIVATE ${PROP_TARGET})