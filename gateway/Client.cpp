#include "Client.h"
#include <stdlib.h>
#include "ClientMgr.h"
#include "Backend.h"
#include "BackendMgr.h"

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) ;
void on_read(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf);

uint32_t Client::_alloc_id_ =0;

void Client::free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t*) req;
    uv_buf_t* curr = wr->bufs_;
    for(uint32_t i=0; i<wr->bufs_size_; ++i)
    {
        free(curr[i].base);
    }
    free(wr->bufs_);
    free(wr);
}



int Client::init(uv_loop_t* loop)
{
    int ires = uv_tcp_init(loop, &tcp_);
    if(ires !=0 )
        return ires;
    tcp_.data=this;
    

    return 0;
}

void Client::async_write()
{
    write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
    uint32_t buf_size = res_queue_.size();
    req->bufs_ = (uv_buf_t*)malloc(sizeof(uv_buf_t)*buf_size);
    req->bufs_size_ = buf_size;
    for(uint32_t i=0;i<buf_size;++i)
    {
        req->bufs_[i]=res_queue_.front();
        res_queue_.pop();
    }
    uv_write((uv_write_t*) req, native_uv<uv_stream_t>(), req->bufs_, req->bufs_size_, /*on_write*/ [](uv_write_t *req, int status) {
        if (status) {
            fprintf(stderr, "Write error %s\n", uv_strerror(status));
        }
        Client::free_write_req(req);
    });
}

int Client::async_read()
{
    return uv_read_start(native_uv<uv_stream_t>(), alloc_buffer, on_read);
}


void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    Client* client = reinterpret_cast<Client*>(handle->data);
    if(client->is_recving_pkg_len())
    {
        printf("client[%u] alloc head! suggested[%lu]\n",client->id(),suggested_size);
        buf->base = client->recv_pkg_len_buf();
        buf->len = client->recv_pkg_len_buf_size();
        printf("base[%p] len[%lu]\n", buf->base, buf->len);
    }
    else
    {
        printf("client[%u] alloc body len[%u]! suggested[%lu]\n",
            client->id(),client->recv_pkg_len(), suggested_size);
        buf->base = (char*) malloc(client->recv_pkg_len());
        buf->len = client->recv_pkg_len();
    }
}
void on_read(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
    Client* client = reinterpret_cast<Client*>(handle->data);
    if (nread > 0) {
        if(client->is_recving_pkg_len())
        {
            printf("read head len[%ld]\n", nread);
            if(nread == buf->len)
            {
                //ntoh 
                client->recving_pkg();
                //client->read();
            }
            else
            {
                //fuck
            }
        }
        else
        {
            uint32_t client_id = client->id();
            printf("client[%u] read body len[%ld] {%.*s}\n", client_id, nread, nread, buf->base);
            // client_req_t req={*buf, client->id()};
            // g_client_req_queue.push(req);
            
            BackendMgr::instance().Get(client->backend_id())->send(client_id, buf->base, buf->len);
            free(buf->base);
            client->recving_pkg_len();
            
        }
        return;
    }
    else if (nread < 0) {
        switch(nread)
        {
        case UV_EOF:
            break;
        case UV_ENOBUFS:
            //client->read();
            break;
        default: 
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
            uv_close((uv_handle_t*)handle, [](uv_handle_t* handle) {
                Client* client = reinterpret_cast<Client*>(handle->data);
                ClientMgr::instance().Free(client->id());
            });
            break;
        }
        
            
    }
    else 
    {
        printf("client[%u] read size[%ld]\n", client->id(), nread);
        if( client->is_recving_pkg_len() )
            return ;
    }

    free(buf->base);
}