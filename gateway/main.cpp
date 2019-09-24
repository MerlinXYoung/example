#include <iostream>

#include <uv.h>
#include <curl/curl.h>
#include <uvcurl/Multi.hpp>

#include "Acceptor.h"
#include "BackendMgr.h"


using namespace std;
#define DEFAULT_PORT 20801



std::unique_ptr<uvcurl::Multi> g_multi;

int main(int args, char** argv)
{
    auto loop = uv_default_loop();
    uint32_t id=343443443;
    Acceptor acceptor;
    acceptor.init(*loop);
    acceptor.listen("0.0.0.0", DEFAULT_PORT);


    BackendMgr::instance().Alloc(0x010000F1, "tcp://127.0.0.1:20802");

    uv_prepare_t prepare_zmq;
    uv_prepare_init(loop, &prepare_zmq);
    uv_prepare_start(&prepare_zmq, [](uv_prepare_t* handle){
        BackendMgr::instance().recv();
    });
    // auth  curl
    if (curl_global_init(CURL_GLOBAL_ALL)) {
        fprintf(stderr, "Could not init cURL\n");
        return 1;
    }

    //uvcurl::Multi multi;
    g_multi = std::make_unique<uvcurl::Multi>(loop);


    return uv_run(loop, UV_RUN_DEFAULT);
}