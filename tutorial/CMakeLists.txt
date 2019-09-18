
file(GLOB SRCS "*.cpp")
#message(STATUS "SRCS:${SRCS}")
link_directories(${PROJECT_BINARY_DIR}/uvactor)
foreach(src ${SRCS})
    #message("src:${src}")
    string(REPLACE ".cpp" "" target ${src})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" target ${target})
    #message(STATUS "target:${target}")
    add_executable(${target} ${src})
    target_link_libraries(${target} zmq libgonet libgo boost_context boost_regex boost_system boost_thread pthread dl)
    
endforeach(src ${SRCS})

add_subdirectory(backend)
add_subdirectory(client)
