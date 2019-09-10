#pragma once 

//#include <uvpp/uvpp.hpp>
#include <uv.h>
namespace uvactor
{
class Acceptor
{
public:
    Acceptor(){}
    ~Acceptor(){}
    bool Init(uv_loop_t& loop);
    bool Listen(const char* ip, int port, uv_connection_cb cb);

protected:
    uv_tcp_t tcp_;
};

}