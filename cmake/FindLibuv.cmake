
find_path(Libuv_INCLUDE_DIR
    uv.h
    PATHS /usr/include /usr/local/include ${Libuv_DIR}/include
)

find_library(Libuv_LIBRARIES
    uv
    PATHS /usr/lib /usr/local/lib ${Libuv_DIR}/lib
)
message(STATUS "Libuv:${Libuv_INCLUDE_DIR} ${Libuv_LIBRARIES}")
string(COMPARE NOTEQUAL "${Libuv_INCLUDE_DIR}"  "" Libuv_INCLUDE_DIR-NOTFOUND)
string(COMPARE NOTEQUAL "${Libuv_LIBRARIES}"  "" Libuv_LIBRARIES-NOTFOUND)
#message(STATUS "Libuv:${Libuv_INCLUDE_DIR-NOTFOUND} ${Libuv_LIBRARIES-NOTFOUND}")
if(Libuv_INCLUDE_DIR-NOTFOUND)
    message(WARNING "Libuv_INCLUDE_DIR-NOTFOUND")
    set(Libuv-NOTFOUND ON)
elseif(Libuv_LIBRARIES-NOTFOUND)
    message(WARNING "Libuv_LIBRARIES-NOTFOUND")
    set(Libuv-NOTFOUND ON)
else()
    set(Libuv_FOUND ON)
    string(REPLACE "/libuv.a" "" Libuv_LIBRARY_DIR ${Libuv_LIBRARIES})
    string(COMPARE EQUAL ${Libuv_LIBRARY_DIR} ${Libuv_LIBRARIES} _EQUAL)
    if(_EQUAL)
    string(REPLACE "/libuv.so" "" Libuv_LIBRARY_DIR ${Libuv_LIBRARIES})
    endif()
    message(STATUS "Libuv_LIBRARY_DIR:${Libuv_LIBRARY_DIR}")
endif()