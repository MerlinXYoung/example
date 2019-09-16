cmake_minimum_required(VERSION 3.10)
project(example)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(CMAKE_CXX_FLAGS "-g -ggdb" )


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

set(Zmq_DIR /usr/local/zeromq)
find_package(Zmq REQUIRED)
include_directories(${Zmq_INCLUDE_DIR})
link_directories(${Zmq_LIBRARY_DIR})

set(Libuv_DIR /usr/local/libuv)
find_package(Libuv REQUIRED)
include_directories(${Libuv_INCLUDE_DIR})
link_directories(${Libuv_LIBRARY_DIR})

add_subdirectory(gateway)
add_subdirectory(tutorial)