#include "ClientMgr.h"
#include "Client.h"
#include "stdex.h"
#include "log.h"
ClientMgr& ClientMgr::instance()
{
    static ClientMgr _instance;
    return _instance;
}

Client* ClientMgr::Alloc()
{
    log_trace("");
    Client *client = new Client;
    client_map_.emplace(client->id(), 
        std::unique_ptr<Client>(client));
    return client;
}
void ClientMgr::Free(uint32_t id)
{
    client_map_.erase(id);
}
Client* ClientMgr::Get(uint32_t id)
{
    auto it = client_map_.find(id);
    return (it == client_map_.end()?nullptr:it->second.get());

}