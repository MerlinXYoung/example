#include <cs_gateway_utility.h>
#include <arpa/inet.h>
#include "../log.h"
namespace gw
{
namespace cs
{

bool ReqParser::parse(const std::vector<char>& pkg)
{
    head_ = std::make_shared<Head>();

    if(pkg.size()< sizeof(uint16_t))
        return false ;
    const char* curr = pkg.data()+sizeof(uint16_t);
    uint32_t len = pkg.size() - sizeof(uint16_t);

    uint16_t head_len = ntohs(*reinterpret_cast<const uint16_t*>(pkg.data()));
    if(len<head_len)
        return false ;

    if(!head_->ParseFromArray(curr, head_len))
        return false;
    curr+=head_len;
    len-=head_len;
    switch(head_->msgid())
    {
        default:
            return false;
        case EMsgID::Other:
        {
            data_.data=curr;
            data_.len=len;
            return true;
        }
        case EMsgID::Auth:
            body_ = std::shared_ptr<google::protobuf::Message>(reinterpret_cast<google::protobuf::Message*>(new gw::cs::AuthReq));
            break;
        case EMsgID::Ping:
            body_ = std::shared_ptr<google::protobuf::Message>(reinterpret_cast<google::protobuf::Message*>(new gw::cs::PingReq));
            break;
    }

    if(!body_->ParseFromArray(curr, len))
        return false;
    return true;
    
}


}
}