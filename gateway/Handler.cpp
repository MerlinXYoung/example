#include "Handler.h"
#include "log.h"
#include "QQAuth.h"
using namespace gw::cs;
int CSHandler::doProcess(uint32_t client_id, const Head& head, google::protobuf::Message& msg)
{
    switch(head.msgid())
    {
    case EMsgID::Auth:
    {
        return doAuth(client_id, head, static_cast<gw::cs::AuthReq&>(msg));   
    }
    default:
        log_error( "fuck msgid[%d]", head.msgid());
    }
}
int CSHandler::doAuth(uint32_t client_id, const gw::cs::Head& head, gw::cs::AuthReq& req)
{
    QQAuth auth;
    return auth.do_auth(client_id, req.openid().c_str(), req.openkey().c_str());

}

CSHandler cshandler;