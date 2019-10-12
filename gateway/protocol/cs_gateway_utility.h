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
    using head_ptr = std::shared_ptr<Head>;
    using msg_ptr = std::shared_ptr<google::protobuf::Message>;
    struct Data
    {
        const char* data;
        uint32_t len;
    };

    ReqParser(){}
    ~ReqParser()=default;
    bool parse(const std::vector<char>& pkg);


    inline head_ptr head(){return head_;}
    inline msg_ptr body(){return body_;}
    inline Data data(){return data_;}

private:
    //uint16_t head_len_;
    //uint32_t body_len_;

    head_ptr head_;
    
    msg_ptr body_;
    Data data_;


};

class ReqPacker
{

};

}
}