


file(GLOB_RECURSE HEAD_FILES "*.h" "*.hpp")
file(GLOB_RECURSE SRC_FILES "*.cpp")


add_executable(gateway ${HEAD_FILES} ${SRC_FILES})
target_link_libraries(gateway uv zmq)