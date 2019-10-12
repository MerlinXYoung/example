#pragma once
#include <unordered_map>
#include <memory>
#include "stdex.h"
#include <boost/serialization/singleton.hpp>
#include "Client.h"

typedef struct uv_loop_s uv_loop_t;
class ClientMgr :public boost::serialization::singleton<ClientMgr>
{
public:
    friend class boost::serialization::detail::singleton_wrapper<ClientMgr>;
    using client_ptr = Client::pointer;
    using  client_map_t = std::unordered_map<uint32_t, client_ptr> ;
    //static ClientMgr& instance();

    client_ptr Alloc(uv_loop_t* loop);
    void Free(uint32_t id);
    client_ptr Get(uint32_t id);
private:
    ClientMgr()=default;
    ~ClientMgr()=default;
    //NONE_COPYABLE(ClientMgr);
private:
    client_map_t  client_map_;
};