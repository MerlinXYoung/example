cmake_minimum_required(VERSION 3.10)
project(example)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(CMAKE_CXX_FLAGS "-g -ggdb" )

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

set(Zmq_DIR /usr/local/zeromq)
find_package(Zmq REQUIRED)
include_directories(${Zmq_INCLUDE_DIR})
link_directories(${Zmq_LIBRARY_DIR})

set(Libuv_DIR /usr/local/libuv)
find_package(Libuv REQUIRED)
include_directories(${Libuv_INCLUDE_DIR})
link_directories(${Libuv_LIBRARY_DIR})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

set(Protobuf_INCLUDE_DIR /usr/local/protobuf/include)
find_package(Protobuf REQUIRED

)
include_directories(${Protobuf_INCLUDE_DIRS})
link_directories(/usr/local/protobuf/lib)

set(Libgo_DIR /usr/local/libgo)
find_package(Libgo REQUIRED)
include_directories(${Libgo_INCLUDE_DIR})
link_directories(${Libgo_LIBRARY_DIR})

set(Libgonet_DIR /usr/local/libgonet)
find_package(Libgonet REQUIRED)
include_directories(${Libgonet_INCLUDE_DIR})
link_directories(${Libgonet_LIBRARY_DIR})

add_subdirectory(uvcurl)
add_subdirectory(gateway)
add_subdirectory(tutorial)