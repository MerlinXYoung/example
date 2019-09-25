#pragma once

#include <zmq.hpp>
#include <string>
#include <unordered_map>
#include "noncopyable.hpp"
#include "../build/gateway/ss_gateway.pb.h"
class Client;
class IBackend:public noncopyable
{
public:
    virtual void connect(uint32_t id, const std::string& url) = 0;
    virtual void send(const gw::ss::Head& head, const char* data, size_t size)=0;
};
class Backend: public IBackend
{
public:
    friend class BackendMgr;
    // using zsocket_ptr = std::unique_ptr<zmq::socket_t>;
    // using zsocket_map_t = std::unordered_map<uint32_t, zsocket_ptr> ;
    Backend(zmq::context_t& ctx):id_(0),zsocket_(ctx, ZMQ_DEALER){}
    virtual void connect(uint32_t id, const std::string& url) override;

    

    void send_client_new(Client& client);
    void send_client_auth(Client& client, const char* openid, const char* openkey );
    void send_client_close(Client& client);
    void send_client_other(Client& client, const char* data, size_t size);
    Client* recv();
    uint32_t id()const
    {
        return id_;
    }
private:
    virtual void send(const gw::ss::Head& head, const char* data, size_t size) override;
    void send(Client& client, gw::ss::EMsgID msgid, const char* data, size_t size);
    std::string set_id(uint32_t id);
private:
    //uint32_t recv_cnt;
    uint32_t id_;
    //zmq::context_t ctx_;
    //zsocket_map_t zsockets_;
    zmq::socket_t zsocket_;
};