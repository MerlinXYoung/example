#include "Acceptor.h"
#include "Client.h"
#include "ClientMgr.h"
#include "BackendMgr.h"
#include "Backend.h"
#include "log.h"

#define DEFAULT_BACKLOG 128
int Acceptor::init(uv_loop_t& loop)
{
    return uv_tcp_init(&loop, &server_);
}

int Acceptor::listen(const char* ip, int port)
{
    struct sockaddr_in addr;
    uv_ip4_addr(ip, port, &addr);
    uv_tcp_bind(&server_, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*) &server_, DEFAULT_BACKLOG, [](uv_stream_t *server, int status) {
        log_trace("");
        if (status < 0) {
            log_error( "New connection error %s\n", uv_strerror(status));
            // error!
            return;
        }

        Client *client = ClientMgr::instance().Alloc();
        log_trace("");
        client->init(server->loop);
        log_trace("");
        client->set_backend_id(BackendMgr::instance().Get()->id());
        log_trace("");
        if (uv_accept(server, client->native_uv<uv_stream_t>()) == 0) {
            log_trace("accept new client[%u]\n", client->id());
            client->async_read();
        }
        else {
            client->async_close();
        }
    });
    if (r) {
        log_error( "Listen error %s\n", uv_strerror(r));
        return 1;
    }
    return 0;
}