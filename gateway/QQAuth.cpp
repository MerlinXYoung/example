#include "QQAuth.h"
#include <curl/curl.h>
#include <sstream>
#include <uvcurl/Multi.hpp>
#include <memory>
#include <assert.h>
#include "log.h"
#include "ClientMgr.h"
#include "Client.h"

// extern std::unique_ptr<uvcurl::Multi> g_multi;
typedef size_t (*curl_writefunction_t)(char* ptr, size_t size, size_t nmemb, void* userdata);
int QQAuth::do_auth(uint32_t client_id, const char* openid, const char* appkey) 
{
    time_t now = time(NULL);
    char sig[100];
    snprintf(sig, sizeof(sig),"%s%ld", qq_appkey, now);
    std::stringstream ss;
    ss<<qq_auth_url<<"?timestamp="<<now<<"&appid="<<qq_appid<<"&sig="<<sig<<"&openid="<<openid<<"&openkey="<<appkey;//<<"&userip"
    std::string url = ss.str();
    CURL *handle = curl_easy_init();

    auto context = new qq_auth_context_t(client_id, handle);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, (curl_writefunction_t)([](char *ptr, size_t size, size_t nmemb, void *userdata)->size_t{
        log_trace("ptr[%p] size[%lu] nmemb[%lu] data[%p]", ptr, size, nmemb, userdata);
        size_t xSize = size*nmemb;
        std::string * pStr = reinterpret_cast<std::string*>(userdata);
        if ( pStr )
            pStr->append(ptr, xSize);

        return xSize;
    }));
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&context->data_));
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

    //curl_multi_add_handle(curl_handle, handle);
    multi_->async_preform(handle, [context](CURL* curl){
        assert(curl == context->curl_);
        std::unique_ptr<qq_auth_context_t> guard(context);
        long res_code =0; 
        char *done_url;
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL,
                            &done_url);
        int res=curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
        log_trace("client[%u] url[%s] http_code[%ld]", context->client_id_, done_url, res_code);
        auto client = ClientMgr::get_mutable_instance().Get(context->client_id_);
        if(!client)
            return ;
        //正确响应后，请请求转写成本地文件的文件
        if(( res == CURLE_OK ) && (res_code == 200 || res_code == 201))
        {
            log_trace("http rsp[%s]", context->data_.c_str());
            client->auth_cb(0);
        }


    });

    fprintf(stderr, "Added %s \n", url.c_str());
}

IAuth* QQAuthFactory::create()
{
    auto auth = new QQAuth;
    if(!auth)
        return nullptr;
    auth->set_multi(multi_);
    return auth;
}

void QQAuthFactory::destroy(IAuth* auth)
{
    delete auth;
}
