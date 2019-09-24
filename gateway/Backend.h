#pragma once

#include <zmq.hpp>
#include <string>
#include <unordered_map>
#include "noncopyable.hpp"
class Client;
class IBackend:public noncopyable
{
public:
    virtual void connect(uint32_t id, const std::string& url) = 0;
    virtual void send(uint32_t client_id, const char* data, size_t size)=0;
};
class Backend: public IBackend
{
public:
    friend class BackendMgr;
    // using zsocket_ptr = std::unique_ptr<zmq::socket_t>;
    // using zsocket_map_t = std::unordered_map<uint32_t, zsocket_ptr> ;
    Backend():ctx_(1),zsocket_(ctx_, ZMQ_DEALER){}
    void connect(uint32_t id, const std::string& url);

    void send(uint32_t client_id, const char* data, size_t size);
    Client* recv();
    uint32_t id()const
    {
        return id_;
    }
private:
    std::string set_id(uint32_t id);
private:
    //uint32_t recv_cnt;
    uint32_t id_;
    zmq::context_t ctx_;
    //zsocket_map_t zsockets_;
    zmq::socket_t zsocket_;
};