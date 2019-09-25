#include "Backend.h"
#include <iomanip>
#include <sstream>
#include <unordered_set>
#include "Client.h"
#include "ClientMgr.h"
#include "log.h"
#include "../build/gateway/ss_gateway.pb.h"

void Backend::connect(uint32_t id, const std::string& url) {
  id_ = id;
  zsocket_.connect(url);
  set_id(id);
}
std::string Backend::set_id(uint32_t id) {
  std::stringstream ss;
  ss << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << id;
  zsocket_.setsockopt(ZMQ_IDENTITY, ss.str().c_str(), ss.str().length());
  return ss.str();
}
void Backend::send(const gw::ss::Head& head, const char* data, size_t size) {
  //zsocket_.send(zmq::message_t((void*)&client_id, sizeof(client_id)), ZMQ_SNDMORE);
  std::string strHead;
  head.SerializePartialToString(&strHead);
  if(size>0)
  {
    zsocket_.send(zmq::message_t(strHead.data(), strHead.size()), ZMQ_SNDMORE); 
    zsocket_.send(zmq::message_t(data, size));
  }
  else
    zsocket_.send(zmq::message_t(strHead.data(), strHead.size())); 
}

void Backend::send(Client& client, gw::ss::EMsgID msgid, const char* data, size_t size) {
  gw::ss::Head head;
  head.set_msgid(msgid);
  head.set_client_id(client.id());
  head.set_uid(client.uid());
  this->send( head, data, size);

}

void Backend::send_client_new(Client& client)
{
  log_trace("client id[%u]", client.id());
  gw::ss::ClientNewReq req;
  std::string strReq;
  req.SerializePartialToString (&strReq);
  this->send(client, gw::ss::EMsgID::ClientNew, strReq.data(), strReq.length() );

}
void Backend::send_client_auth(Client& client, const char* openid, const char* openkey )
{
  log_trace("client id[%u]", client.id());
  gw::ss::ClientAuthReq req;
  req.set_openid(openid);
  req.set_openkey(openkey);
  std::string strReq;
  req.SerializePartialToString (&strReq);
  this->send(client, gw::ss::EMsgID::ClientAuth, strReq.data(), strReq.length() );
}
void Backend::send_client_close(Client& client)
{
  log_trace("client id[%u]", client.id());
  gw::ss::ClientCloseReq req;
  std::string strReq;
  req.SerializePartialToString (&strReq);
  this->send(client, gw::ss::EMsgID::ClientAuth, strReq.data(), strReq.length() );
}

void Backend::send_client_other(Client& client, const char* data, size_t size)
{
  this->send(client, gw::ss::EMsgID::Other, data, size );
}

Client* Backend::recv() {
  try {
    int more = 0;
    uv_buf_t buf = {0};
    zmq::message_t message_client;
    zsocket_.recv(message_client);
    size_t more_size = sizeof(more);
    zsocket_.getsockopt(ZMQ_RCVMORE, &more, &more_size);
    int size = message_client.size();
    if (!more || size != sizeof(uint32_t)) {
      log_error("fuck message not more or  not client id");
      return nullptr;
    }
    // std::string str_client_id(reinterpret_cast<char *>(message.data()),
    // size);
    uint32_t client_id = *reinterpret_cast<uint32_t*>(
        message_client.data());  // atol(str_client_id.c_str());
    zmq::message_t message;
    zsocket_.recv(message);
    size = message.size();
    buf.base = (char*)malloc(size + sizeof(uint32_t));
    buf.len = size + sizeof(uint32_t);
    const char* d = reinterpret_cast<char*>(message.data());
    uint32_t nlen = htonl(size);
    memcpy(buf.base, &nlen, sizeof(nlen));
    memcpy(buf.base + sizeof(uint32_t), message.data(), size);
    std::string data(d, size);
    log_trace("msg[%u]:%s\n", client_id, data.c_str());
    auto client = ClientMgr::instance().Get(client_id);
    if (client) {
      client->async_write(buf);
    }
    return client;
  } catch (std::exception& e) {
  }
  return nullptr;
}
