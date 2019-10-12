#pragma once
#include <stdint.h>
class IAuth
{
public:
    virtual ~IAuth(){}
    virtual int do_auth(uint32_t client_id, const char* openid, const char* appkey)=0;
};


class IAuthFactory
{
public:
    virtual IAuth* create()=0;
    virtual void destroy(IAuth* )=0;
};
