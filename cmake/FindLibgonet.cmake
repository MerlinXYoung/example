
find_path(Libgonet_INCLUDE_DIR
    libgonet/network.h
    PATHS /usr/include /usr/local/include ${Libgonet_DIR}/include
)

find_library(Libgonet_LIBRARIES
    libgonet
    PATHS /usr/lib /usr/local/lib ${Libgonet_DIR}/lib
)
message(STATUS "Libgonet:${Libgonet_INCLUDE_DIR} ${Libgonet_LIBRARIES}")
string(COMPARE NOTEQUAL "${Libgonet_INCLUDE_DIR}"  "" Libgonet_INCLUDE_DIR-NOTFOUND)
string(COMPARE NOTEQUAL "${Libgonet_LIBRARIES}"  "" Libgonet_LIBRARIES-NOTFOUND)
#message(STATUS "Libgo:${Libgonet_INCLUDE_DIR-NOTFOUND} ${Libgonet_LIBRARIES-NOTFOUND}")
if(Libgonet_INCLUDE_DIR-NOTFOUND)
    message(WARNING "Libgonet_INCLUDE_DIR-NOTFOUND")
    set(Libgo-NOTFOUND ON)
elseif(Libgonet_LIBRARIES-NOTFOUND)
    message(WARNING "Libgonet_LIBRARIES-NOTFOUND")
    set(Libgo-NOTFOUND ON)
else()
    set(Libgonet_FOUND ON)
    string(REPLACE "/liblibgonet.a" "" Libgonet_LIBRARY_DIR ${Libgonet_LIBRARIES})
    string(COMPARE EQUAL ${Libgonet_LIBRARY_DIR} ${Libgonet_LIBRARIES} _EQUAL)
    if(_EQUAL)
        string(REPLACE "/liblibgonet.so" "" Libgonet_LIBRARY_DIR ${Libgonet_LIBRARIES})
    endif()
    message(STATUS "Libgonet_LIBRARY_DIR:${Libgonet_LIBRARY_DIR}")
endif()