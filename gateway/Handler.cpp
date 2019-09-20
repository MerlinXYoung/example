#include "Handler.h"
#include "log.h"
using namespace gw::cs;
int CSHandler::doProcess(uint32_t client_id, const Head& head, google::protobuf::Message& msg)
{
    switch(head.msgid())
    {
    case EMsgID::Auth:
    {
        
    }
    default:
        log_error( "fuck msgid[%d]", head.msgid());
    }
}

CSHandler cshandler;