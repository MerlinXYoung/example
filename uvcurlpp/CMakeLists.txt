
file(GLOB_RECURSE HEAD_FILES "*.h")
file(GLOB_RECURSE SRC_FILES "*.cpp")


add_library(uvcurlpp ${HEAD_FILES} ${SRC_FILES})
