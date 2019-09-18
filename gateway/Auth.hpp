#pragma once
#include <stdint.h>
class IAuth
{
public:
    virtual int do_auth(uint32_t client_id, const char* openid, const char* appkey)=0;
};

