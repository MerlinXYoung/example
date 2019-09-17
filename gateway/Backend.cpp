#include "Backend.h"
#include <sstream>
#include <iomanip>
#include <unordered_set>
#include "Client.h"
#include "ClientMgr.h"
#include "log.h"

void Backend::connect(uint32_t id, const std::string& url)
{
    id_ = id;
    zsocket_.connect(url);
    set_id(id);
}
std::string Backend::set_id(uint32_t id)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase
        << std::setw(4) << std::setfill('0') << id;
    zsocket_.setsockopt(ZMQ_IDENTITY, ss.str().c_str(), ss.str().length());
    return ss.str();
}
void Backend::send(uint32_t client_id, const char* data, size_t size)
{
    zsocket_.send((void*)&client_id, sizeof(client_id), ZMQ_SNDMORE);
    zsocket_.send(data, size);
}
#if 0
void Backend::recv()
{
    
    zmq::pollitem_t items[] = {
        { static_cast<void*>(zsocket_), 0, ZMQ_POLLIN, 0 } 
    };
    try {
        std::unordered_set<Client*> clients;
        for (int i = 0; i < recv_cnt; ++i) {
            // 10 milliseconds
            zmq::poll(items, 1, 0);
            if (items[0].revents & ZMQ_POLLIN) {
                // log_trace("\n%s ", identity);
                // s_dump(client_socket_);
                uv_buf_t buf;
                zmq::message_t message;
                zsocket_.recv(&message);
                int size = message.size();
                std::string str_client_id(static_cast<char*>(message.data()), sizeof(uint32_t));
                uint32_t client_id = atol(str_client_id.c_str());
                buf.base = (char*)malloc(size-sizeof(uint32_t));
                buf.len = size-sizeof(uint32_t);
                memcpy(buf.base, message.data()+sizeof(uint32_t), buf.len);
                
                std::string data(static_cast<char*>(message.data()+sizeof(uint32_t)), size-sizeof(uint32_t));
                log_trace("msg[%u]:%s\n", client_id, data.c_str());
                auto client = ClientMgr::instance().Get(client_id);
                if(client)
                {
                    client->async_write(buf);
                    clients.insert(client);
                }

            }
            else
            {
                break;
            }
        }
        for( auto cli : clients)
        {
            cli->async_write();
        }

    }
    catch (std::exception &e) {}
    
}
#else
Client* Backend::recv()
{

    try
    {
        int more=0;
        uv_buf_t buf={0};
        zmq::message_t message_client;
        zsocket_.recv(&message_client);
        size_t more_size = sizeof (more);
        zsocket_.getsockopt(ZMQ_RCVMORE, &more, &more_size);
        int size = message_client.size();
        if(!more || size != sizeof(uint32_t))
        {
            log_error("fuck message not more or  not client id");
            return nullptr;
        }
        //std::string str_client_id(reinterpret_cast<char *>(message.data()), size);
        uint32_t client_id = *reinterpret_cast<uint32_t*>(message_client.data());//atol(str_client_id.c_str());
        zmq::message_t message;
        zsocket_.recv(&message);
        size = message.size();
        buf.base = (char *)malloc(size+sizeof(uint32_t));
        buf.len = size+sizeof(uint32_t);
        const char* d= reinterpret_cast<char *>(message.data());
        uint32_t nlen = htonl(size);
        memcpy(buf.base, &nlen, sizeof(nlen));
        memcpy(buf.base+sizeof(uint32_t), message.data(), size);
        std::string data(d, size);
        log_trace("msg[%u]:%s\n", client_id, data.c_str());
        auto client = ClientMgr::instance().Get(client_id);
        if (client)
        {
            client->async_write(buf);

        }
        return client;
    }
    catch (std::exception &e)
    {
    }
    return nullptr;
}
#endif