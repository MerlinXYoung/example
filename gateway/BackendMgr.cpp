#include "BackendMgr.h"
#include "Backend.h"
#include "Client.h"
#include <vector>
#include <unordered_set>

BackendMgr& BackendMgr::instance()
{
    static BackendMgr _instance;
    return _instance;
}
BackendMgr::backend_ptr BackendMgr::Alloc(uint32_t id, const std::string& url)
{
    //Backend* backend = new Backend;
    auto ptr = std::shared_ptr<Backend>(new Backend);// std::make_shared<Backend>();
    ptr->connect(id, url);
    backends_array_.push_back(ptr);
    backends_.insert(std::make_pair(id, ptr));
}
BackendMgr::backend_ptr BackendMgr::Get(uint32_t id)
{
    auto it = backends_.find(id);
    return it == backends_.end()?nullptr:it->second;
}

BackendMgr::backend_ptr BackendMgr::Get()
{
    uint32_t idx=0;
    return backends_array_[idx++%backends_array_.size()];
}

void BackendMgr::recv()
{
    
    // zmq::pollitem_t items[] = {
    //     { static_cast<void*>(zsocket_), 0, ZMQ_POLLIN, 0 } 
    // };
    
    try {
        std::vector<zmq::pollitem_t> zitem_array;    
        for(auto it:backends_array_ )    
        {
            zitem_array.push_back({static_cast<void*>(it->zsocket_), 0, ZMQ_POLLIN, 0});    
        }
        
        std::unordered_set<Client*> clients;
        for (int i = 0; i < recv_cnt_; ++i) {
            // 10 milliseconds
            std::vector<zmq::pollitem_t> zitems(zitem_array);
            zmq::poll(zitems.data(), zitems.size(), 0);
            for(size_t j=0; j<zitems.size(); ++j)
            {
                if (zitems[j].revents & ZMQ_POLLIN) {
                    // printf("\n%s ", identity);
                    // s_dump(client_socket_);
                    auto ptr = backends_array_[j];
                    Client* client = ptr->recv();
                    if(client)
                    {
                        clients.insert(client);
                    }

                }
            }
        }
        for( auto cli : clients)
        {
            cli->async_write();
        }

    }
    catch (std::exception &e) {}
}