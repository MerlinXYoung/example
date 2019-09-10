#pragma once

#include <uvpp/handle.hpp>
#include <uvpp/error.hpp>
#include <uvpp/callback.hpp>
#include <chrono>

namespace uvpp {
class Timer : public handle<uv_timer_t>
{
public:
    Timer():
        handle<uv_timer_t>()
    {
        uv_timer_init(uv_default_loop(), native());
    }

    Timer(loop& l):
        handle<uv_timer_t>()
    {
        uv_timer_init(l.native(), native());
    }

    error start(Callback callback, const std::chrono::duration<uint64_t, std::milli> &timeout, const std::chrono::duration<uint64_t, std::milli> &repeat)
    {

        return error(uv_timer_start(native(),
                                    [callback](uv_timer_t* handle)
        {
            callback();
        },
        timeout.count(),
        repeat.count()
                                   ));
    }

    error start(Callback callback, const std::chrono::duration<uint64_t, std::milli> &timeout)
    {

        return error(uv_timer_start(native(),
                                    [callback](uv_timer_t* handle)
        {
            callback();
        },
        timeout.count(),
        0
                                   ));
    }

    error stop()
    {
        return error(uv_timer_stop(native()));
    }

    error again()
    {
        return error(uv_timer_again(native()));
    }
};
}