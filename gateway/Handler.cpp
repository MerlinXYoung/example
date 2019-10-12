#include "Handler.h"
#include "log.h"
#include "QQAuth.h"
#include "BackendMgr.h"
#include "ClientMgr.h"
#include "cs_gateway_utility.h"
using namespace gw::cs;
int CSHandler::doProcess(Client::pointer client, const Client::pkg_t& pkg)
{
  try{
    do{
      ReqParser parser;
      if(!parser.parse(pkg))
      {
        break;
      }
      if(parser.head()->msgid() == EMsgID::Other)
      {
        if(!client->is_authed())
        {
          // client->async_close();
          ClientMgr::get_mutable_instance().Free(client->id());
          return 0;
        }
        log_trace("body[%.*s],",parser.data().len, parser.data().data );
        auto backend = BackendMgr::get_mutable_instance()
            .Get(client->backend_id());
          if(backend)
            backend->send_client_other(*client, parser.data().data, parser.data().len);
      }
    
      return doProcess(client, *parser.head(), *parser.body());

    }while(0);
    return -1;
  }catch(std::exception & e)
  {
    log_error( "exception:%s", e.what());
  }
  catch(...)
  {

  }
}

int CSHandler::doProcess(Client::pointer client, const Head& head, google::protobuf::Message& msg)
{
    switch(head.msgid())
    {
    case EMsgID::Auth:
    {
        return doAuth(client, head, static_cast<gw::cs::AuthReq&>(msg));   
    }
    default:
        log_error( "fuck msgid[%d]", head.msgid());
    }
}
int CSHandler::doAuth(Client::pointer client, const gw::cs::Head& head, gw::cs::AuthReq& req)
{
    QQAuth auth;
    return auth.do_auth(client->id(), req.openid().c_str(), req.openkey().c_str());

}

CSHandler cshandler;