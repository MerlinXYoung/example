#pragma once
#include <uv.h>
#include <curl/curl.h>
#include <memory>
#include <unordered_map>
#include <functional>

namespace uvcurlpp
{
class Multi final
{
public:
    using done_cb_t = std::function<void(CURL*)> ;
    using curlm_ptr = std::unique_ptr<CURLM,CURLMcode(*)(CURLM*)>;
    using done_cb_map_t = std::unordered_map<CURL*, done_cb_t>;
    Multi()=delete;
    Multi(uv_loop_t* loop = uv_default_loop());
    ~Multi();
    //int  init(uv_loop_t& loop);
    int async_preform(CURL* handle, done_cb_t done_cb);

    void curl_done(CURL* handle);

public:
    inline CURLM* get_curlm()
    {
        return curlm_.get();
    }
    inline uv_loop_t* get_uvloop()
    {
        return loop_;
    }
    inline uv_timer_t* get_timer()
    {
        return &timer_;
    }
private:
    uv_loop_t * loop_;  
    //CURLM *     curl_handle_; 
    curlm_ptr   curlm_; 
    uv_timer_t  timer_; 
    done_cb_map_t done_cb_map_;

};
}