
file(GLOB SRCS "*.cpp")
file(GLOB HDS "*.h")
set(PROTO_FILE ${PROJECT_SOURCE_DIR}/gateway/protocol/ss_gateway.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTO_FILE})
add_executable(backend ${SRCS} ${HDS} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(backend libzmq protobuf)
