#pragma once

#include <uv.h>
#include <queue>

class Client
{
private:
    typedef struct {
        uv_write_t req_;
        uint32_t bufs_size_;
        uv_buf_t* bufs_;

    } write_req_t;
    
    static void free_write_req(uv_write_t *req);
    
public:
    friend class ClientMgr;
    using res_queue_t = std::queue<uv_buf_t>;
    Client()
    {
        recving_pkg_len_=true;
        id_ = alloc_id();
    }
    int init(uv_loop_t* loop);

    template<typename UV_T>
    UV_T* native_uv()
    {
        return reinterpret_cast<UV_T*>(&tcp_);
    }
    inline uint32_t id()const 
    {
        return id_;
    }
    inline uint32_t backend_id()const
    {
        return backend_id_;
    }
    void set_backend_id(uint32_t backend_id);
    void async_write(uv_buf_t buf)
    {
        res_queue_.push(buf);
    }
    void async_write();
    int async_read();

    inline bool is_recving_pkg_len()const
    {
        return recving_pkg_len_;
    }
    inline void recving_pkg()
    {
        recving_pkg_len_=false;
    }
    inline void recving_pkg_len()
    {
        recving_pkg_len_=true;
    }
    inline char* recv_pkg_len_buf()
    {
        return reinterpret_cast<char*>(&recv_pkg_len_);
    }
    inline size_t recv_pkg_len_buf_size()const
    {
        return sizeof(recv_pkg_len_);
    }
    inline uint32_t recv_pkg_len()const
    {
        return recv_pkg_len_;
    }
    void ntoh_body_len();
private:
    static inline uint32_t alloc_id()
    {
        return ++_alloc_id_;
    }
    static uint32_t _alloc_id_ ;
protected:
    uv_tcp_t tcp_;
    uint32_t id_;
    uint32_t backend_id_;
    uint32_t recv_pkg_len_;
    bool recving_pkg_len_;
    res_queue_t res_queue_;


};
