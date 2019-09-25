#pragma once
#include "../../build/gateway/cs_gateway.pb.h"
#include <stdint.h>
namespace gw
{
namespace cs
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
    inline uint32_t body_len()const{return pkg_len_ - sizeof(uint16_t) -head_len_;}
    inline char* body(){return pkg_+sizeof(uint16_t)+head_len_;}

private:
    char* pkg_;
    uint32_t pkg_len_;
    uint64_t head_len_;
    //uint32_t body_len_;

};

class ReqPacker
{

};

}
}