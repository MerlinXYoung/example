#include <iostream>

#include <uv.h>

#include "Acceptor.h"
#include "BackendMgr.h"


using namespace std;
#define DEFAULT_PORT 20801

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

    return uv_run(loop, UV_RUN_DEFAULT);
}