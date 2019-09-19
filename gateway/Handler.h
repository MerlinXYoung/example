#pragma once
#include "cs_gateway.pb.h"

class CSHandler
{
public:
    int doHandle(uint32_t client_id, gw::cs::EMsgID msgid, google::protobuf::message& msg);
private:
    int doAuth(uint32_t client_id, gw::cs::EMsgID msgid, google::protobuf::message& msg);

};

extern CSHandler cshandler;