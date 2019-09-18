
file(GLOB SRCS "*.cpp")
file(GLOB HDS "*.h")


add_executable(client ${SRCS} ${HDS})
target_link_libraries(client libgonet libgo boost_context boost_regex boost_system boost_thread pthread dl)
    



