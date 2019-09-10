#pragma once

#include <uvpp/handle.hpp>
#include <uvpp/error.hpp>

namespace uvpp {

class Poll : public handle<uv_poll_t>
{
public:
    Poll(int fd):
        handle<uv_poll_t>()
    {
        uv_poll_init(uv_default_loop(), native(),fd);
    }

    Poll(loop& l, int fd):
        handle<uv_poll_t>()
    {
        uv_poll_init(l.native(), native(), fd);
    }

    error start( int events, std::function<void(int status,int events)> callback)
    {
        return error(uv_poll_start(native(), events,
                                   [callback](uv_poll_t* handle, int status, int events)
        {
            callback( status, events);
        }
                                  ));
    }

    error stop()
    {
        return error(uv_poll_stop(native()));
    }
};
}