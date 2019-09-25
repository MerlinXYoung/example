
file(GLOB SRCS "*.cpp")
file(GLOB HDS "*.h")


add_executable(backend ${SRCS} ${HDS} ${PROJECT_BINARY_DIR}/gateway/ss_gateway.pb.cc ${PROJECT_BINARY_DIR}/gateway/ss_gateway.pb.h)
target_link_libraries(backend libzmq protobuf)
