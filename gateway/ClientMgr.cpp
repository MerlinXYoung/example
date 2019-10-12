#include "ClientMgr.h"
#include "Client.h"
#include "stdex.h"
#include "log.h"
// ClientMgr& ClientMgr::instance()
// {
//     static ClientMgr _instance;
//     return _instance;
// }

Client::pointer ClientMgr::Alloc(uv_loop_t* loop)
{
    log_trace("");
    // Client *client = new Client;
    // client_map_.emplace(client->id(), 
    //     std::unique_ptr<Client>(client));
    Client::pointer client(new Client(loop));
    client_map_.insert(std::make_pair(client->id(), client));
    return client;
}
void ClientMgr::Free(uint32_t id)
{
    client_map_.erase(id);
}
Client::pointer ClientMgr::Get(uint32_t id)
{
    auto it = client_map_.find(id);
    return (it == client_map_.end()?std::shared_ptr<Client>(nullptr):it->second);

}