#include "Client.h"
#include <arpa/inet.h>
#include <cstdlib>
#include "Backend.h"
#include "BackendMgr.h"
#include "ClientMgr.h"
#include "Handler.h"
#include "protocol/cs_gateway_utility.h"
#include "log.h"
#include "stdex.h"
#include <stdexcept>

using namespace std;
#define CLIENT_UV_BUF_SIZE (1024)
struct write_req_t{
    uv_write_t req_;
    vector<uv_buf_t> bufs_;
    write_req_t ():req_{},bufs_(){
      req_.data = this;
    }
    write_req_t (Client::res_queue_t& res_queue)
    {
      req_.data = this;
      while(!res_queue.empty())
      {
        bufs_.push_back(res_queue.front());
        res_queue.pop();
      }
      
    }
    ~write_req_t() = default;
  };
static void _uv_tcp_close(uv_tcp_t* tcp)
{
  if(nullptr != tcp)
    return;
  if(tcp->loop)
  {
    uv_close(reinterpret_cast<uv_handle_t*>(tcp),[](uv_handle_t* handle){
      free(handle);
    });
  }
  else{
    free(tcp);
  }
}
uint32_t Client::_alloc_id_ = 0;
Client::Client(uv_loop_t* loop):id_(alloc_id()),backend_id_(0),uid_(0),status_(EStatus::New),
  tcp_ptr_(new uv_tcp_t,_uv_tcp_close),/*recv_buf_(),*/pkg_len_(0),curr_pkg_(),res_queue_()  {

  int ires = uv_tcp_init(loop, tcp_ptr_.get());
  if (ires != 0) throw std::runtime_error("uv_tcp_init error!");
  tcp_ptr_->data = this;

  status_ = EStatus::New;
}

Client::~Client()
{

}

void Client::async_write() {
  write_req_t *req = new write_req_t(res_queue_);
  uv_write(&req->req_, native_uv<uv_stream_t>(), req->bufs_.data(),
           req->bufs_.size(), /*on_write*/ [](uv_write_t *req, int status) {
             if (status) {
               log_error("Write error %s\n", uv_strerror(status));
             }
             auto wreq = reinterpret_cast<write_req_t*>(req->data);
             delete wreq;
           });
}

int Client::async_read() {
  return uv_read_start(native_uv<uv_stream_t>(), 
    [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
        // auto client = reinterpret_cast<Client *>(handle->data)->shared_from_this();
        // client->get_buf(*buf);
        // log_trace("buf[%p] data[%p] len[%lu]", buf, buf->base, buf->len);
        buf->base = new char[CLIENT_UV_BUF_SIZE];//reinterpret_cast<char*>(malloc(CLIENT_UV_BUF_SIZE));
        buf->len = CLIENT_UV_BUF_SIZE;
    }, 
    [](uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
        log_trace("handle[%p] nread[%ld] buf[%p] data[%p] len[%lu]", handle, nread, buf, buf->base, buf->len);
        // auto _guard = std::make_unique(buf->base, free);
        std::unique_ptr<char[]> _guard(buf->base);
        auto client = reinterpret_cast<Client *>(handle->data)->shared_from_this();
        if (nread > 0) {
          
          client->recved(buf->base, nread);
          return;
        } else if (nread < 0) {
            switch (nread) {
            case UV_EOF:
                break;
            case UV_ENOBUFS:
                // client->read();
                break;
            default:
                log_error("Read error %s\n", uv_err_name(nread));
                ClientMgr::get_mutable_instance().Free(client->id());
                break;
            }
        } else {
            log_trace("client[%u] read size[%ld]\n", client->id(), nread);
        }
    });
}
// void Client::async_close() {
//   uv_close(native_uv<uv_handle_t>(), [](uv_handle_t *handle) {
//     auto client = reinterpret_cast<Client *>(handle->data)->shared_from_this();
//     ClientMgr::get_mutable_instance().Free(client->id());
//   });
// }

// void Client::set_backend_id(uint32_t backend_id) {
//   log_trace("backend_id:%u", backend_id);
//   backend_id_ = backend_id;
// }


// void Client::get_buf(uv_buf_t &buf) {
//   recv_buf_.get(buf);
// }
void Client::auth_cb(int status) {
  log_trace("auth status[%d]", status);
  if (0 != status) 
  {
    //async_close();
    ClientMgr::get_mutable_instance().Free(id());
    return ;
  }
  status_ = EStatus::Authed;
  set_uid(rand());
  BackendMgr::get_mutable_instance().Get(backend_id())->send_client_auth(*this, "openid", "openkey");
    
}

void Client::recv_pkg_len(const char*& data, uint32_t& len)
{
  uint32_t space = sizeof(uint32_t) -curr_pkg_.size();
  if(space <= len)
  {
    curr_pkg_.insert(curr_pkg_.end(), data, data+space);
    pkg_len_ = ntohl(*reinterpret_cast<uint32_t*>(curr_pkg_.data()));
    log_trace("pkg_len:%u", pkg_len_);
    curr_pkg_.clear();
    data += space;
    len -= space;
  }
  else 
  {
    curr_pkg_.insert(curr_pkg_.end(), data, data+len);
    data += len;
    len = 0;
  }
}
void Client::recved(const char* data, uint32_t len) {
  const char* curr = data;
  do{
    if(pkg_len_ == 0 )
    {
      recv_pkg_len(curr, len);
      continue;
    }
    log_trace("pkg_len[%u] cached[%lu] recved[%u]", pkg_len_, curr_pkg_.size(), len);
    uint32_t space = pkg_len_-curr_pkg_.size();
    if(space> len)
    {
      curr_pkg_.insert(curr_pkg_.end(), curr, curr+len);
      break;
    }
    else 
    {
      curr_pkg_.insert(curr_pkg_.end(), curr, curr+space);
      curr += space;
      len -= space;
      pkg_len_ = 0;
      cshandler.doProcess(shared_from_this(), curr_pkg_);
    }
  }while(true);
}

