#include "Acceptor.h"
#include "Client.h"
#include "ClientMgr.h"
#include "BackendMgr.h"
#include "Backend.h"

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
        if (status < 0) {
            fprintf(stderr, "New connection error %s\n", uv_strerror(status));
            // error!
            return;
        }

        Client *client = ClientMgr::instance().Alloc();
        client->init(server->loop);
        client->set_backend_id(BackendMgr::instance().Get()->id());
        if (uv_accept(server, client->native_uv<uv_stream_t>()) == 0) {
            printf("accept new client[%u]\n", client->id());
            client->async_read();
        }
        else {
            uv_close(client->native_uv<uv_handle_t>(), [](uv_handle_t* handle) {
                Client* client = reinterpret_cast<Client*>(handle->data);
                ClientMgr::instance().Free(client->id());
            });
        }
    });
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }
}