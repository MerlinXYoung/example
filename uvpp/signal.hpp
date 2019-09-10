#pragma once

#include <uvpp/handle.hpp>
#include <uvpp/error.hpp>

namespace uvpp {

class Signal : public handle<uv_signal_t>
{
public:
    
    typedef std::function<void(int sugnum)> SignalHandler;
    
    Signal():
        handle<uv_signal_t>()
    {
        uv_signal_init(uv_default_loop(), native());
    }

    Signal(loop& l):
        handle<uv_signal_t>()
    {
        uv_signal_init(l.native(), native());
    }


    error start(int signum, SignalHandler callback)
    {
        return error(uv_signal_start(native(),
                                     [callback](uv_signal_t* handle, int signum)
        {
            callback(signum);
        },
        signum));
    }

    error stop()
    {
        return error(uv_signal_stop(native()));
    }
};
}
