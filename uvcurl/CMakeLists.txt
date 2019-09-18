
file(GLOB_RECURSE HEAD_FILES "*.h")
file(GLOB_RECURSE SRC_FILES "*.cpp")


add_library(uvcurl ${HEAD_FILES} ${SRC_FILES})