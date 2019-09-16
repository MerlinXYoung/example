#pragma once

#include <uv.h>

class Acceptor
{
public:
    Acceptor(){}
    ~Acceptor(){}

    int init(uv_loop_t& loop);
    int listen(const char* ip, int port);
    //int async_accept();

private:
    uv_tcp_t server_;
};