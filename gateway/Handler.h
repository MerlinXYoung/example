#pragma once
#include "../build/gateway/cs_gateway.pb.h"

class CSHandler
{
public:
    int doProcess(uint32_t client_id, const gw::cs::Head& head, google::protobuf::Message& msg);
private:
    int doAuth(uint32_t client_id, const gw::cs::Head& head, gw::cs::AuthReq& req);

};

extern CSHandler cshandler;