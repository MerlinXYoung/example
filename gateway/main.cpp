#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <queue>
#include <sstream>
#include <iomanip>

#include <uv.h>
#include <zmq.hpp>



using namespace std;
#define DEFAULT_PORT 20801
#define DEFAULT_BACKLOG 128
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) ;
void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
void on_write(uv_write_t *req, int status);
typedef struct {
    uv_write_t req_;
    uint32_t bufs_size_;
    uv_buf_t* bufs_;

} write_req_t;


void free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t*) req;
    uv_buf_t* curr = wr->bufs_;
    for(uint32_t i=0; i<wr->bufs_size_; ++i)
    {
        free(curr[i].base);
    }
    free(wr->bufs_);
    free(wr);
}

class Client
{
public:
    using res_queue_t = std::queue<uv_buf_t>;
    Client()
    {
        recving_pkg_len_=true;
    }
    int init(uv_loop_t* loop)
    {
        int ires = uv_tcp_init(loop, &tcp_);
        if(ires !=0 )
            return ires;
        tcp_.data=this;
        id_ = alloc_id();
    }

    template<typename UV_T>
    UV_T* native_uv()
    {
        return reinterpret_cast<UV_T*>(&tcp_);
    }
    inline uint32_t id()const 
    {
        return id_;
    }
    void write(uv_buf_t buf)
    {
        res_queue_.push(buf);
    }
    void write()
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
        uv_write((uv_write_t*) req, native_uv<uv_stream_t>(), req->bufs_, req->bufs_size_, on_write);
    }
    int read()
    {
        return uv_read_start(native_uv<uv_stream_t>(), alloc_buffer, on_read);
    }

    inline bool is_recving_pkg_len()const
    {
        return recving_pkg_len_;
    }
    inline void recving_pkg()
    {
        recving_pkg_len_=false;
    }
    inline void recving_pkg_len()
    {
        recving_pkg_len_=true;
    }
    inline char* recv_pkg_len_buf()
    {
        return reinterpret_cast<char*>(&recv_pkg_len_);
    }
    inline size_t recv_pkg_len_buf_size()const
    {
        return sizeof(recv_pkg_len_);
    }
    inline uint32_t recv_pkg_len()const
    {
        return recv_pkg_len_;
    }
private:
    static inline uint32_t alloc_id()
    {
        return ++_alloc_id_;
    }
    static uint32_t _alloc_id_ ;
protected:
    uv_tcp_t tcp_;
    uint32_t id_;
    uint32_t recv_pkg_len_;
    bool recving_pkg_len_;
    res_queue_t res_queue_;


};
uint32_t Client::_alloc_id_ =0;

using  client_map_t = std::unordered_map<uint32_t, std::unique_ptr<Client>> ;

static client_map_t g_client_map;

typedef struct
{
    uv_buf_t buf_;
    uint32_t client_id_;
} client_req_t;

// void free_client_req(client_req_t *req) {
//     write_req_t *wr = (write_req_t*) req;
//     free(wr->buf.base);
//     free(wr);
// }

using  client_req_queue_t = std::queue<client_req_t>;
client_req_queue_t g_client_req_queue;





void on_close(uv_handle_t* handle) {
    Client* client = reinterpret_cast<Client*>(handle->data);
    g_client_map.erase(client->id());
}

void on_write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    free_write_req(req);
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
            printf("read body len[%ld] {%.*s}\n", nread, nread, buf->base);
            client_req_t req={*buf, client->id()};
            g_client_req_queue.push(req);
            client->recving_pkg_len();
            // write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
            // req->buf = uv_buf_init(buf->base, nread);
            // uv_write((uv_write_t*) req, client, &req->buf, 1, echo_write);
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
            uv_close((uv_handle_t*) client, on_close);
            break;
        }
        
            
    }

    free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }

    Client *client = new Client;//(client_t*) malloc(sizeof(client_t));
    client->init(server->loop);
    g_client_map.emplace(client->id(), std::unique_ptr<Client>(client));
    if (uv_accept(server, client->native_uv<uv_stream_t>()) == 0) {
        printf("accept new client[%u]\n", client->id());
        uv_read_start(client->native_uv<uv_stream_t>(), alloc_buffer, on_read);
    }
    else {
        uv_close(client->native_uv<uv_handle_t>(), on_close);
    }
}
class backend_task
{
public:
    backend_task():ctx_(1),backend_(ctx_, ZMQ_DEALER){}
    void connect(const std::string url)
    {
        backend_.connect(url);
    }
    inline std::string set_id(uint32_t id)
    {
        std::stringstream ss;
        ss << std::hex << std::uppercase
            << std::setw(4) << std::setfill('0') << id;
        backend_.setsockopt(ZMQ_IDENTITY, ss.str().c_str(), ss.str().length());
        return ss.str();
    }
    void send()
    {
        while(!g_client_req_queue.empty())
        {
            auto req = g_client_req_queue.front();
            backend_.send((void*)&req.client_id_, 4, ZMQ_SNDMORE);
            backend_.send(req.buf_.base, req.buf_.len);
            free(req.buf_.base);
            g_client_req_queue.pop();
        }
    }
    void recv()
    {
        
        zmq::pollitem_t items[] = {
            { static_cast<void*>(backend_), 0, ZMQ_POLLIN, 0 } 
        };
        try {
            std::unordered_set<Client*> clients;
            for (int i = 0; i < recv_cnt; ++i) {
                // 10 milliseconds
                zmq::poll(items, 1, 0);
                if (items[0].revents & ZMQ_POLLIN) {
                    // printf("\n%s ", identity);
                    // s_dump(client_socket_);
                    uv_buf_t buf;
                    zmq::message_t message;
                    backend_.recv(&message);
                    int size = message.size();
                    std::string str_client_id(static_cast<char*>(message.data()), sizeof(uint32_t));
                    uint32_t client_id = atol(str_client_id.c_str());
                    buf.base = (char*)malloc(size-sizeof(uint32_t));
                    buf.len = size-sizeof(uint32_t);
                    memcpy(buf.base, message.data()+sizeof(uint32_t), buf.len);
                    
                    std::string data(static_cast<char*>(message.data()+sizeof(uint32_t)), size-sizeof(uint32_t));
                    printf("msg[%u]:%s\n", client_id, data.c_str());
                    auto it = g_client_map.find(client_id);
                    if(it != g_client_map.end())
                    {
                        it->second->write(buf);
                        clients.insert(it->second.get());
                    }

                }
                else
                {
                    break;
                }
            }
            for( auto cli : clients)
            {
                cli->write();
            }

        }
        catch (std::exception &e) {}
        
    }
private:
    uint32_t recv_cnt;
    zmq::context_t ctx_;
    zmq::socket_t backend_;
};
backend_task g_backend_task;
void on_zmq(uv_prepare_t* handle)
{
    g_backend_task.send();
    g_backend_task.recv();
}

int main(int args, char** argv)
{
    auto loop = uv_default_loop();
    uint32_t id=343443443;
    uv_tcp_t server;
    uv_tcp_init(loop, &server);
    struct sockaddr_in addr;

    uv_ip4_addr("0.0.0.0", DEFAULT_PORT, &addr);

    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*) &server, DEFAULT_BACKLOG, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }

    g_backend_task.connect("tcp://127.0.0.1:20802");
    g_backend_task.set_id(id);

    uv_prepare_t prepare_zmq;
    uv_prepare_init(loop, &prepare_zmq);
    uv_prepare_start(&prepare_zmq, on_zmq);

    return uv_run(loop, UV_RUN_DEFAULT);
}