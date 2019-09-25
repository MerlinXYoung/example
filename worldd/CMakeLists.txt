
file(GLOB_RECURSE SRCS "*.cpp")
file(GLOB_RECURSE HDS "*.h" "*.hpp")

add_executable(worldd ${SRCS} ${HDS})