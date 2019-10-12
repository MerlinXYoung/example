#pragma once

#include <uv.h>
#include <queue>
#include "Buffer.h"
#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>

class Client final:public boost::noncopyable, public std::enable_shared_from_this<Client> {
  public:
    enum class EStatus {
      New = 0,
      Authing,
      Authed,
    };
  friend class ClientMgr;
  //friend class std::shared_ptr<Client>;
  using res_queue_t = std::queue<uv_buf_t>;
  //using recv_buf_t = std::vector<char>;
  using pkg_t = std::vector<char>;
  //using recv_buf_t = Buffer<char>;
  using pointer = std::shared_ptr<Client>;
  using uv_tcp_ptr = std::unique_ptr<uv_tcp_t,void (*)(uv_tcp_t* handle)>;
 protected:

  Client(uv_loop_t* loop=uv_default_loop()) ;
  
 public:
  ~Client();
  
  // int init(uv_loop_t* loop);

  template <typename UV_T>
  UV_T* native_uv() {
    return reinterpret_cast<UV_T*>(tcp_ptr_.get());
  }
  inline uint32_t id() const { return id_; }
  inline uint32_t backend_id() const { return backend_id_; }
  inline void set_backend_id(uint32_t backend_id){
    backend_id_ = backend_id;
  }
  inline void set_uid(uint64_t uid)
  {
    uid_ = uid;
  }
  inline uint64_t uid()const{return uid_;}

  inline void async_write(uv_buf_t buf) { res_queue_.push(buf); }
  void async_write();
  int async_read();
  // void async_close();

  void auth_cb(int status);

  //void get_buf(uv_buf_t& buf);
  void recved(const char* data, uint32_t len);

  inline bool is_authed()const{ 
    status_ == EStatus::Authed;
  }
private:
void recv_pkg_len(const char*& data, uint32_t& len);
 private:
  static inline uint32_t alloc_id() { return ++_alloc_id_; }
  static uint32_t _alloc_id_;

 protected:
  uint32_t id_;
  uint32_t backend_id_;
  uint64_t uid_;
  EStatus status_;
  // uv_tcp_t* tcp_;
  uv_tcp_ptr tcp_ptr_;
  // recv_buf_t recv_buf_;
  // uv_buf_t uv_buf_;
  uint32_t pkg_len_;
  pkg_t curr_pkg_;
  res_queue_t res_queue_;
};
