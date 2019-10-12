
file(GLOB SRCS "*.cpp")
file(GLOB HDS "*.h")


add_executable(backend ${SRCS} ${HDS} )
target_link_libraries(backend gwpb zmq protobuf)
