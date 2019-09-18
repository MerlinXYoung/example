


file(GLOB_RECURSE PROTO_FILES "*.proto")
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})


file(GLOB_RECURSE HEAD_FILES "*.h" "*.hpp")
file(GLOB_RECURSE SRC_FILES "*.cpp")


add_executable(gateway ${HEAD_FILES} ${SRC_FILES} ${PROTO_HDRS} ${PROTO_SRCS})
target_link_libraries(gateway uvcurl uv curl zmq protobuf)
