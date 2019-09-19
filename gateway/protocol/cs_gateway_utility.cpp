#include "cs_gateway_utlity.h"

namespace gw
{
namespace cs
{


int64_t ReqParser::check(char* base, uint32_t len)
{
    uint32_t body_len = 0 ;
    if(len< sizeof(uint32_t))
        return 0 ;
    
    body_len = ntohl(*reinterpret_cast<uint32_t*>(base));
    base += sizeof(uint32_t);
    len -= sizeof(uint32_t);

    if(len < body_len)
        return 0 ;
    
    pkg_ = base;
    pkg_len_ = body_len;

    return body_len+sizeof(uint32_t);
}
EMsgID ReqParser::parse(Head& head, google::protobuf::message*& msg)
{
    uint16_t head_len = 0; 
    if(pkg_len_< sizeof(uint16_t))
        return EMsgID::Invalid ;
    head_len = ntoh(*reinterpret_cast<uint16_t*>(pkg_));

    head.Parse
    
}


}
}