#pragma once

#include "stdex.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <zmq.hpp>
#include <boost/serialization/singleton.hpp>

#include "Backend.h"
class BackendMgr :public boost::serialization::singleton<BackendMgr>
{
public:
    
    using backend_ptr = Backend::pointer;
    using backend_map_t = std::unordered_map<uint32_t, backend_ptr>;
    using backend_array_t = std::vector<backend_ptr>;
    //friend class boost::serialization::detail::singleton_wrapper<BackendMgr>;
    //static BackendMgr& instance();

    backend_ptr Alloc(uint32_t id, const std::string& url);
    backend_ptr Get(uint32_t id);

    backend_ptr Get();
    
    void recv();

protected:
    BackendMgr():ctx_(1),recv_cnt_(1000){};
    ~BackendMgr()=default;
    // NONE_COPYABLE(BackendMgr);
private:
    zmq::context_t ctx_;
    uint32_t recv_cnt_;
    backend_map_t backends_;
    backend_array_t backends_array_;

};