cmake_minimum_required(VERSION 3.4)
project(Lo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")

set(SOURCE_FILES main.cpp lo.cpp lo.h pthread_pool.cpp pthread_pool.h http.cpp http.h)
add_executable(Lo ${SOURCE_FILES})

target_link_libraries(Lo pthread)
