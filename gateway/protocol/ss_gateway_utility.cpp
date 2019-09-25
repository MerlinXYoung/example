#include <ss_gateway_utility.h>
#include "../log.h"
namespace gw
{
namespace ss
{


int64_t ReqParser::check(char* base, uint32_t len)
{
    uint32_t body_len = 0 ;
    if(len< sizeof(uint32_t))
        return 0 ;
    
    body_len = *reinterpret_cast<uint32_t*>(base);
    base += sizeof(uint32_t);
    len -= sizeof(uint32_t);

    if(len < body_len)
        return 0 ;
    
    pkg_ = base;
    pkg_len_ = body_len;
    char buf[1024];
    char *curr = buf;
    for(int i=0;i<body_len-sizeof(uint16_t);++i)
    {
        size_t n = sprintf(curr, "%2X ", static_cast<int>(*(pkg_+sizeof(uint16_t)+i)));
        curr+=n;
    }
    log_trace("%s",buf); 


    return body_len+sizeof(uint32_t);
}
EMsgID ReqParser::parse(Head& head, google::protobuf::Message*& msg)
{
    uint16_t head_len = 0; 
    if(pkg_len_< sizeof(uint16_t))
        return EMsgID::Invalid ;
    head_len = *reinterpret_cast<uint16_t*>(pkg_);
    char* curr = pkg_+sizeof(uint16_t);
    if(!head.ParseFromArray(curr, head_len))
        return EMsgID::Invalid;
    curr+=head_len;
    // if(EMsgID::Auth == head.msgid())
    //     msg = new gw::cs::AuthReq;
    // if(!msg->ParseFromArray(curr, pkg_len_ - head_len -sizeof(uint16_t)))
    //     return EMsgID::Invalid;
    return head.msgid();
    
}


}
}