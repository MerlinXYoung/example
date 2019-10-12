


file(GLOB_RECURSE PROTOCOL_PROTO_FILES "protocol/*.proto")
PROTOBUF_GENERATE_CPP(PROTOCOL_PB_SRCS PROTOCOL_PB_HDRS ${PROTOCOL_PROTO_FILES})

file(GLOB_RECURSE CFG_PROTO_FILES "cfg_desc/*.proto")
PROTOBUF_GENERATE_CPP(CFG_PB_SRCS CFG_PB_HDRS ${CFG_PROTO_FILES})


file(GLOB_RECURSE HEAD_FILES "*.h" "*.hpp")
file(GLOB_RECURSE SRC_FILES "*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/protocol)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/pb2jsonpp)

# add_library(gwpb ${CFG_PB_HDRS} ${CFG_PB_SRCS})

add_library(gwpb ${PROTOCOL_PB_HDRS} ${PROTOCOL_PB_SRCS})
add_executable(gateway ${HEAD_FILES} ${SRC_FILES} ${CFG_PB_HDRS} ${CFG_PB_SRCS} )
target_link_libraries(gateway gwpb uvcurl libuv pb2json curl zmq protobuf gflags boost_serialization)
set_target_properties(gateway PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime/gateway/bin )