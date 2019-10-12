cmake_minimum_required(VERSION 3.15)
project(example)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(CMAKE_CXX_FLAGS "-g -ggdb -Wdeprecated-declarations" )
set(CMAKE_C_FLAGS "-g -ggdb" )

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/libzmq/share/cmake)

#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/local/libzmq /usr/local/cppzmq)
include(prefix.cmake)
message(STATUS "CMAKE_PREFIX_PATH:${CMAKE_PREFIX_PATH}")
# find_package(ZeroMQ REQUIRED)
# include_directories(${ZeroMQ_INCLUDE_DIR})
# link_directories(${ZeroMQ_LIBRARY_DIR})


set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/local/openssl)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})


#set(Zmq_DIR /usr/local/zeromq)
find_package(Zmq REQUIRED)
include_directories(${Zmq_INCLUDE_DIR})
link_directories(${Zmq_LIBRARY_DIR})

find_package(cppzmq REQUIRED)
include_directories(${cppzmq_INCLUDE_DIR})


find_package(Libuv 1.10 REQUIRED )
include_directories(${Libuv_INCLUDE_DIR})


find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/local/protobuf)
#set(Protobuf_INCLUDE_DIR /usr/local/protobuf/include)
find_package(Protobuf 3.0 REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
link_directories(/usr/local/protobuf/lib)

find_package(gflags REQUIRED)
include_directories(${gflags_INCLUDE_DIR})
# link_directories(/usr/local/protobuf/lib)

set(Libgo_DIR /usr/local/libgo)
find_package(Libgo REQUIRED)
include_directories(${Libgo_INCLUDE_DIR})
link_directories(${Libgo_LIBRARY_DIR})

#set(Libgonet_DIR /usr/local/libgonet)
#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/local/libgonet)
find_package(Libgonet REQUIRED)
include_directories(${Libgonet_INCLUDE_DIR})
link_directories(${Libgonet_LIBRARY_DIR})

include_directories(${PROJECT_SOURCE_DIR}/uvcurl)

add_subdirectory(pb2jsonpp)
add_subdirectory(uvcurl)
# add_subdirectory(uvcurlpp)
add_subdirectory(gateway)
add_subdirectory(tutorial)
