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

using namespace std;

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

uint32_t Client::_alloc_id_ = 0;

int Client::init(uv_loop_t *loop) {
  int ires = uv_tcp_init(loop, &tcp_);
  if (ires != 0) return ires;
  tcp_.data = this;

  status_ = EStatus::Inited;

  return 0;
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
        Client *client = reinterpret_cast<Client *>(handle->data);
        client->get_buf(*buf);
        log_trace("buf[%p] data[%p] len[%lu]", buf, buf->base, buf->len);
    }, 
    [](uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
        log_trace("handle[%p] nread[%ld] buf[%p] data[%p] len[%lu]", handle, nread, buf, buf->base, buf->len);
        Client *client = reinterpret_cast<Client *>(handle->data);
        if (nread > 0) {
            client->recved(nread);
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
                client->async_close();
                break;
            }
        } else {
            log_trace("client[%u] read size[%ld]\n", client->id(), nread);
        }
    });
}
void Client::async_close() {
  uv_close(native_uv<uv_handle_t>(), [](uv_handle_t *handle) {
    Client *client = reinterpret_cast<Client *>(handle->data);
    ClientMgr::instance().Free(client->id());
  });
}

// void Client::set_backend_id(uint32_t backend_id) {
//   log_trace("backend_id:%u", backend_id);
//   backend_id_ = backend_id;
// }


void Client::get_buf(uv_buf_t &buf) {
  recv_buf_.get(buf);
}
void Client::auth_cb(int status) {
  log_trace("auth status[%d]", status);
  if (0 != status) 
  {
    async_close();
  }
  status_ = EStatus::Authed;
  set_uid(rand());
  BackendMgr::instance().Get(backend_id())->send_client_auth(*this, "openid", "openkey");
    
}


void Client::recved(ssize_t len) {
  log_trace("recved[%lu]", len)
  recv_buf_.use(len);
  char *curr = recv_buf_.data();
  char *curr_base = recv_buf_.data();
  uint32_t buf_len = recv_buf_.len();
  gw::cs::ReqParser parser;
  do {
    int64_t full_pkg_len = parser.check(curr, buf_len);
    if (full_pkg_len == 0) break;
    if (full_pkg_len < 0) {
      async_close();
      break;
    }
    google::protobuf::Message *message;
    gw::cs::Head head;
    gw::cs::EMsgID msgid = parser.parse(head, message);
    if (gw::cs::EMsgID::Invalid == msgid) {
      async_close();
      break;
    }
    if (gw::cs::EMsgID::Other == msgid)
    {
      if(status_ != EStatus::Authed)
      {
        async_close();
        break;
      }
      log_trace("body[%.*s],",parser.body_len(), parser.body() );
      BackendMgr::instance()
          .Get(backend_id())
          ->send_client_other(*this, parser.body(), parser.body_len());
    }
    else {
      cshandler.doProcess(id_, head, *message);
    }
    delete message;

    curr += full_pkg_len;
    buf_len -= full_pkg_len;
    curr_base = curr;
  } while (buf_len > 0);

  if (curr_base != recv_buf_.data() ) {
    recv_buf_.tidy(recv_buf_.len()-buf_len);
  }
  
}

