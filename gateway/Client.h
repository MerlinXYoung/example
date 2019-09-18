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

    struct recv_buf_t{
        char* base_;
        uint32_t used_;
        uint32_t size_;
        bool is_full()const {return size_ == used_;}
        bool is_empty()const {return size_ == 0;}
        uint32_t remainder()const
        {
            return size_ - used_;
        }
        void use(ssize_t len)
        {
            used_+=len;
        }
        uint32_t get_used()const
        {
            return used_;
        }
        char* curr()
        {
            return base_ + used_;
        }
        uint32_t dilatation();
    } ;

    
    
public:
    enum class EStatus
    {
        New=0,
        Inited, 
        Authing,
        Authed,
    };
    friend class ClientMgr;
    using res_queue_t = std::queue<uv_buf_t>;
    Client()
    {
        recving_pkg_len_=true;
        id_ = alloc_id();
        status_ = EStatus::New;
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
    void async_close();

    void auth_cb(int status);

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

    void try_alloc_recv_buf();
    void get_buf(uv_buf_t& buf);
    void recved(ssize_t len);
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
    EStatus status_;
    uint32_t recv_pkg_len_;
    bool recving_pkg_len_;
    recv_buf_t recv_buf_;
    res_queue_t res_queue_;


};
