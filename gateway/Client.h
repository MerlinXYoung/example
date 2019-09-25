#pragma once

#include <uv.h>
#include <queue>
#include "Buffer.h"

class Client {
  public:
    enum class EStatus {
      New = 0,
      Inited,
      Authing,
      Authed,
    };
  friend class ClientMgr;
  using res_queue_t = std::queue<uv_buf_t>;
  //using recv_buf_t = std::vector<char>;
  using recv_buf_t = Buffer<char>;
 private:
  
 public:

  Client():id_(alloc_id()),backend_id_(0),uid_(0),status_(EStatus::New),tcp_{},recv_buf_(),res_queue_()  {

  }
  int init(uv_loop_t* loop);

  template <typename UV_T>
  UV_T* native_uv() {
    return reinterpret_cast<UV_T*>(&tcp_);
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
  void async_close();

  void auth_cb(int status);

  void get_buf(uv_buf_t& buf);
  void recved(ssize_t len);

 private:
  static inline uint32_t alloc_id() { return ++_alloc_id_; }
  static uint32_t _alloc_id_;

 protected:
  uint32_t id_;
  uint32_t backend_id_;
  uint64_t uid_;
  EStatus status_;
  uv_tcp_t tcp_;
  recv_buf_t recv_buf_;
  res_queue_t res_queue_;
};
