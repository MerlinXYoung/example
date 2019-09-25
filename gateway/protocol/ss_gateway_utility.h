#pragma once
#include "../../build/gateway/ss_gateway.pb.h"
namespace gw
{
namespace ss
{
class ReqParser
{
public:
    ReqParser():pkg_(nullptr),pkg_len_(0){}
    ~ReqParser()=default;
    int64_t check(char* base, uint32_t len);
    EMsgID parse(Head& head, google::protobuf::Message*&);
    inline void reset()
    {
        pkg_ = nullptr;
        pkg_len_ = 0;
    }
    inline char* pkg(){return pkg_;}
    inline uint32_t pkg_len()const{return pkg_len_;}

private:
    char* pkg_;
    uint32_t pkg_len_;
};
}
}