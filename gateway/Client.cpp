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
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void on_read(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf);

uint32_t Client::_alloc_id_ = 0;

void Client::free_write_req(uv_write_t *req) {
  write_req_t *wr = (write_req_t *)req;
  uv_buf_t *curr = wr->bufs_;
  for (uint32_t i = 0; i < wr->bufs_size_; ++i) {
    free(curr[i].base);
  }
  free(wr->bufs_);
  free(wr);
}

uint32_t Client::recv_buf_t::dilatation() {
  uint32_t size = 0;
  if (size_ < 1024)
    size = 1024;
  else if (size_ < 1024 * 4)
    size = size_ * 2;
  else
    size = size_ * 1.75;

  char *base = (char *)malloc(size);
  if (!base) return size_;
  memcpy(base, base_, used_);
  base_ = base;
  size_ = size;
  return size_;
}

int Client::init(uv_loop_t *loop) {
  int ires = uv_tcp_init(loop, &tcp_);
  if (ires != 0) return ires;
  tcp_.data = this;

  std::bzero(recv_buf_);
  status_ = EStatus::Inited;

  return 0;
}

void Client::async_write() {
  write_req_t *req = (write_req_t *)malloc(sizeof(write_req_t));
  uint32_t buf_size = res_queue_.size();
  req->bufs_ = (uv_buf_t *)malloc(sizeof(uv_buf_t) * buf_size);
  req->bufs_size_ = buf_size;
  for (uint32_t i = 0; i < buf_size; ++i) {
    req->bufs_[i] = res_queue_.front();
    res_queue_.pop();
  }
  uv_write((uv_write_t *)req, native_uv<uv_stream_t>(), req->bufs_,
           req->bufs_size_, /*on_write*/ [](uv_write_t *req, int status) {
             if (status) {
               log_error("Write error %s\n", uv_strerror(status));
             }
             Client::free_write_req(req);
           });
}

int Client::async_read() {
  return uv_read_start(native_uv<uv_stream_t>(), 
    [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
        Client *client = reinterpret_cast<Client *>(handle->data);
        client->try_alloc_recv_buf();
        client->get_buf(*buf);
    }, 
    [](uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
        log_trace("");
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

void Client::set_backend_id(uint32_t backend_id) {
  log_trace("backend_id:%u", backend_id);
  backend_id_ = backend_id;
}
void Client::ntoh_body_len() { recv_pkg_len_ = ntohl(recv_pkg_len_); }
void Client::try_alloc_recv_buf() {
  if (recv_buf_.is_full()) recv_buf_.dilatation();
}
void Client::get_buf(uv_buf_t &buf) {
  buf.base = recv_buf_.curr();
  buf.len = recv_buf_.remainder();
}
void Client::auth_cb(int status) {
  log_trace("auth status[%d]", status);
  if (0 == status) status_ = EStatus::Authed;
  async_close();
}
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  Client *client = reinterpret_cast<Client *>(handle->data);
#if 1
  client->try_alloc_recv_buf();
  client->get_buf(*buf);
#else
  if (client->is_recving_pkg_len()) {
    log_trace("client[%u] alloc head! suggested[%lu]\n", client->id(),
              suggested_size);
    buf->base = client->recv_pkg_len_buf();
    buf->len = client->recv_pkg_len_buf_size();
    log_trace("base[%p] len[%lu]\n", buf->base, buf->len);
  } else {
    log_trace("client[%u] alloc body len[%u]! suggested[%lu]\n", client->id(),
              client->recv_pkg_len(), suggested_size);
    buf->base = (char *)malloc(client->recv_pkg_len());
    buf->len = client->recv_pkg_len();
  }
#endif
}
#if 1
void Client::recved(ssize_t len) {
  recv_buf_.use(len);
  char *curr = recv_buf_.base_;
  char *curr_base = recv_buf_.base_;
  uint32_t buf_len = recv_buf_.get_used();
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
      BackendMgr::instance()
          .Get(backend_id())
          ->send(id_, curr + sizeof(uint32_t), full_pkg_len - sizeof(uint32_t));
    else {
      cshandler.doProcess(id_, head, *message);
    }

    curr += full_pkg_len;
    buf_len -= full_pkg_len;
    curr_base = curr;
  } while (buf_len > 0);

  if (curr_base != recv_buf_.base_ && buf_len > 0) {
    memcpy(recv_buf_.base_, curr_base, buf_len);
  }
  recv_buf_.used_ = buf_len;
}

#if 0
void on_read(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
  log_trace("");
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
}
#endif
#else
void on_read(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
  log_trace("");
  Client *client = reinterpret_cast<Client *>(handle->data);
  if (nread > 0) {
    if (client->is_recving_pkg_len()) {
      log_trace("read head len[%ld]\n", nread);
      if (nread == buf->len) {
        // ntoh
        client->ntoh_body_len();
        client->recving_pkg();
        // client->read();
      } else {
        // fuck
      }
    } else {
      log_trace("");
      uint32_t client_id = client->id();
      log_trace("client[%u] read body len[%ld] {%.*s}\n", client_id, nread,
                nread, buf->base);
      // client_req_t req={*buf, client->id()};
      // g_client_req_queue.push(req);

      BackendMgr::instance()
          .Get(client->backend_id())
          ->send(client_id, buf->base, buf->len);
      free(buf->base);
      client->recving_pkg_len();
    }
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
        uv_close((uv_handle_t *)handle, [](uv_handle_t *handle) {
          Client *client = reinterpret_cast<Client *>(handle->data);
          ClientMgr::instance().Free(client->id());
        });
        break;
    }

  } else {
    log_trace("client[%u] read size[%ld]\n", client->id(), nread);
    if (client->is_recving_pkg_len()) return;
  }
  if (!client->is_recving_pkg_len()) free(buf->base);
}
#endif

