#pragma once

#include <uvpp/handle.hpp>
#include <uvpp/error.hpp>
#include <uvpp/file.hpp>

namespace uvpp {

class FsPoll : public handle<uv_fs_poll_t>
{
public:
    typedef std::function<void(error err,int status,Stats prev,Stats current)> FsPoolCallback;
    FsPoll():
        handle<uv_fs_poll_t>()
    {
        uv_fs_poll_init(uv_default_loop(), native());
    }

    FsPoll(loop& l):
        handle<uv_fs_poll_t>()
    {
        uv_fs_poll_init(l.native(), native());
    }

    error start(const std::string &path, unsigned int interval, FsPoolCallback callback)
    {

        return error(uv_fs_poll_start(native(),
                                      [callback](uv_fs_poll_t* handle,  int status, const uv_stat_t* prev, const uv_stat_t* curr)
        {
            Stats back,current;
            if (status<0)
                callback( error(status), status, back, current);
            else
            {
                back = statsFromUV(prev);
                current = statsFromUV(curr);
                callback( error(0), status, back, current);
            }
        }, path.c_str(), interval
                                     ));
    }

    error stop()
    {
        return error(uv_fs_poll_stop(native()));
    }
};
}