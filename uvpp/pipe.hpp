#pragma once

#include <uvpp/stream.hpp>
#include <uvpp/loop.hpp>

namespace uvpp {
class Pipe : public stream<uv_pipe_t>
{
public:
    Pipe(const bool fd_pass = false):
        stream()
    {
        uv_pipe_init(uv_default_loop(), native(), fd_pass ? 1 : 0);
    }

    Pipe(loop& l, const bool fd_pass = false):
        stream()
    {
        uv_pipe_init(l.native(), native(), fd_pass ? 1 : 0);
    }

    bool bind(const std::string& name)
    {
        return uv_pipe_bind(native(), name.c_str()) == 0;
    }

    void connect(const std::string& name, CallbackWithResult callback)
    {
        uv_pipe_connect(new uv_connect_t, native(), name.c_str(), [callback](uv_connect_t* req, int status)
        {
            std::unique_ptr<uv_connect_t> reqHolder(req);
            callback(error(status));
        });
    }

    bool getsockname(std::string& name)
    {
        std::vector<char> buf(100);
        size_t buf_size = buf.size();
        int r = uv_pipe_getsockname(native(), buf.data(), &buf_size);
        if (r == UV_ENOBUFS) {
            buf.resize(buf_size);
            r = uv_pipe_getsockname(native(), buf.data(), &buf_size);
        }

        if (r == 0)
        {
            name = std::string(buf.data(), buf_size);
            return true;
        }
        return false;
    }

    bool getpeername(std::string& name)
    {
        std::vector<char> buf(100);
        size_t buf_size = buf.size();
        int r = uv_pipe_getpeername(native(), buf.data(), &buf_size);
        if (r == UV_ENOBUFS) {
            buf.resize(buf_size);
            r = uv_pipe_getpeername(native(), buf.data(), &buf_size);
        }

        if (r == 0)
        {
            name = std::string(buf.data(), buf_size);
            return true;
        }
        return false;
    }

    int pending_count()
    {
        return uv_pipe_pending_count(native());
    }

    uv_handle_type pending_type()
    {
        return uv_pipe_pending_type(native());
    }
};
}
