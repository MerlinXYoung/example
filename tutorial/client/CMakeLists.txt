
file(GLOB SRCS "*.cpp")
file(GLOB HDS "*.h")

set(PROTO_FILE ${PROJECT_SOURCE_DIR}/gateway/protocol/cs_gateway.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTO_FILE})

add_executable(client ${SRCS} ${HDS} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(client protobuf libgonet libgo boost_context boost_regex boost_system boost_thread pthread dl)
    



