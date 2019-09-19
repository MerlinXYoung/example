#pragma once
#include "cs_gateway.pb.h"
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
    EMsgID parse(Head& head, google::protobuf::message*&);
    inline void reset()
    {
        pkg_ = nullptr;
        pkg_len_ = 0;
    }

private:
    char* pkg_;
    uint32_t pkg_len_;

};

class ReqPacker
{

};

}
}