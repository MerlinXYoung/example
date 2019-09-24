#pragma once
#include "Auth.hpp"
#include <curl/curl.h>
#include <string>

class QQAuth : public IAuth
{
    const char* qq_auth_url="http://ysdktest.qq.com/auth/qq_check_token";
    const char* qq_appid = "1106662470";
    const char* qq_appkey = "ZQMqEM5I4m5jx68q";

    struct qq_auth_context_t
    {
        uint32_t client_id_;
        CURL* curl_;
        std::string data_;
        qq_auth_context_t(uint32_t client_id, CURL* curl):
            client_id_(client_id),curl_(curl),data_(){}
    };
public:
    virtual int do_auth(uint32_t client_id, const char* openid, const char* appkey) override;
};