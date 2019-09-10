
#include <iostream>
#include <uvactor/Acceptor.h>

using namespace std;
using namespace uvactor;
int main(int argc, char** argv)
{
    uv_loop_t* loop = uv_default_loop();
    Acceptor acceptor;
    acceptor.Init(*loop);
    acceptor.Listen("127.0.0.1", 20801, [](uv_stream_t* server, int status){
        uv_tcp_t* new_tcp = new uv_tcp_t;
        uv_tcp_init(server->loop, new_tcp);
        uv_accept(server, reinterpret_cast<uv_stream_t*>( new_tcp));

        cout<<"new clinet:"<<new_tcp->u.fd<<endl;
    });

    uv_run(loop, UV_RUN_DEFAULT) ;

    return 0;
}