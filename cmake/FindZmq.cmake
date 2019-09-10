
find_path(Zmq_INCLUDE_DIR
    zmq.h
    PATHS /usr/include /usr/local/include ${Zmq_DIR}/include
)

find_library(Zmq_LIBRARIES
    zmq
    PATHS /usr/lib /usr/local/lib ${Zmq_DIR}/lib
)
#message(STATUS "Zmg:${Zmq_INCLUDE_DIR} ${Zmq_LIBRARIES}")
string(COMPARE NOTEQUAL "${Zmq_INCLUDE_DIR}"  "" Zmq_INCLUDE_DIR-NOTFOUND)
string(COMPARE NOTEQUAL "${Zmq_LIBRARIES}"  "" Zmq_LIBRARIES-NOTFOUND)
message(STATUS "Zmg:${Zmq_INCLUDE_DIR-NOTFOUND} ${Zmq_LIBRARIES-NOTFOUND}")
if(Zmq_INCLUDE_DIR-NOTFOUND)
    message(WARNING "Zmq_INCLUDE_DIR-NOTFOUND")
    set(Zmg-NOTFOUND ON)
elseif(Zmq_LIBRARIES-NOTFOUND)
    message(WARNING "Zmq_LIBRARIES-NOTFOUND")
    set(Zmg-NOTFOUND ON)
else()
    set(Zmq_FOUND ON)
    string(REPLACE "/libzmq.a" "" Zmq_LIBRARY_DIR ${Zmq_LIBRARIES})
    string(COMPARE EQUAL ${Zmq_LIBRARY_DIR} ${Zmq_LIBRARIES} _EQUAL)
    if(_EQUAL)
    string(REPLACE "/libzmq.so" "" Zmq_LIBRARY_DIR ${Zmq_LIBRARIES})
    endif()
    message(STATUS "Zmq_LIBRARY_DIR:${Zmq_LIBRARY_DIR}")
endif()