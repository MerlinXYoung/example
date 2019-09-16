#pragma once
#include <unordered_map>
#include <memory>
#include "stdex.h"
class Client;
class ClientMgr final
{
public:
    using client_ptr = std::unique_ptr<Client>;
    using  client_map_t = std::unordered_map<uint32_t, client_ptr> ;
    static ClientMgr& instance();

    Client* Alloc();
    void Free(uint32_t id);
    Client* Get(uint32_t id);
private:
    ClientMgr()=default;
    ~ClientMgr()=default;
    NONE_COPYABLE(ClientMgr);
private:
    client_map_t  client_map_;
};