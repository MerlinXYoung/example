#pragma once

#include <uvpp/request.hpp>
#include <uvpp/error.hpp>
#include <uvpp/loop.hpp>

namespace uvpp {
class Idle : public handle<uv_idle_t>
{
public:
    Idle():handle<uv_idle_t>()
    {
        uv_idle_init(uv_default_loop(), native());
    }

    Idle(loop& l):
        handle<uv_idle_t>()
    {
        uv_idle_init(loop.native(), native());
    }

    error start(Callback callback)
    {
        return error(uv_idle_start(native(), [callbock](uv_idle_t* req)
        {
            callback();
        }));

    }

    error stop()
    {
        return error(uv_idle_stop(native()));
    }

private:

};
}



