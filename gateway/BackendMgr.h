#pragma once

#include "stdex.h"
#include <unordered_map>
#include <memory>
#include <vector>
class Backend;
class BackendMgr final
{
public:
    using backend_ptr = std::shared_ptr<Backend>;
    using backend_map_t = std::unordered_map<uint32_t, backend_ptr>;
    using backend_array_t = std::vector<backend_ptr>;
    static BackendMgr& instance();

    backend_ptr Alloc(uint32_t id, const std::string& url);
    backend_ptr Get(uint32_t id);

    backend_ptr Get();
    
    void recv();

private:
    BackendMgr():recv_cnt_(1000){};
    ~BackendMgr()=default;
    NONE_COPYABLE(BackendMgr);
private:
    uint32_t recv_cnt_;
    backend_map_t backends_;
    backend_array_t backends_array_;

};