#include "Handler.h"
using namespace gw::cs;
int CSHandler::doProcess(uint32_t client_id, EMsgID msgid, google::protobuf::message& msg)
{
    switch(msgid)
    {
    case EMsgID::Auth:
    {
        
    }
    default:
        log_error( "fuck msgid[%d]", msgid);
    }
}