#pragma once
#include "cs_gateway.pb.h"
#include "Client.h"
class CSHandler
{
public:
    int doProcess(Client::pointer client, const Client::pkg_t& pkg);
    int doProcess(Client::pointer client, const gw::cs::Head& head, google::protobuf::Message& msg);
private:
    int doAuth(Client::pointer client, const gw::cs::Head& head, gw::cs::AuthReq& req);

};

extern CSHandler cshandler;