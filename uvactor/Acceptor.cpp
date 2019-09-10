#include <uvactor/Acceptor.h>
#include <uvactor/Log.h>
#include <uvactor/Net.hpp>
namespace uvactor
{
bool Acceptor::Init(uv_loop_t& loop)
{
    return uv_tcp_init(&loop, &tcp_)==0;
}
bool Acceptor::Listen(const char* ip, int port, uv_connection_cb cb)
{
    auto addr = to_ip4_addr(ip, port);
    if(0 != uv_tcp_bind(&tcp_, reinterpret_cast<sockaddr*>(&addr), 0))
    {
        uvactor_log("bind[%s][%d] error!", ip, port);
        return false;
    }
    if(0 != uv_listen(reinterpret_cast<uv_stream_t*>(&tcp_), 128, cb))
    {
        uvactor_log("bind[%s][%d] error!", ip, port);
        return false;
    }
    
    return true;
}
}