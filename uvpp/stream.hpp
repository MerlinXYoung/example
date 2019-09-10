#pragma once

#include <uvpp/handle.hpp>
#include <uvpp/error.hpp>
#include <algorithm>
#include <memory>

namespace uvpp {
template<typename HANDLE_T>
class stream : public handle<HANDLE_T>
{
public:
    typedef uv_stream_t native_stream_type;
    typedef uv_stream_t* native_stream_pointer;
    typedef const uv_stream_t* const_native_stream_pointer;

    typedef std::function<void(const char* buf, ssize_t len)> read_callback;
protected:
    stream():
        handle<HANDLE_T>()
    {}

public:
    native_stream_pointer native_stream()
    {
        return reinterpret_cast<native_stream_pointer>(this->native());
    }
    const_native_stream_pointer native_stream()const
    {
        return reinterpret_cast<const_native_stream_pointer>(this->native());
    }
    bool listen(CallbackWithResult callback, int backlog=128)
    {
        m_listen_cb =callback;
        return uv_listen(native_stream(), backlog, [](uv_stream_t* s, int status)
        {
            reinterpret_cast<pointer>(s)->m_listen_cb(error(status));
        }) == 0;
    }

    bool accept(stream& client)
    {
        return uv_accept(native_stream(), client.native_stream()) == 0;
    }

    bool read_start(read_callback callback)
    {
        return read_start<0>(callback);
    }

    template<size_t max_alloc_size>
    bool read_start(read_callback callback)
    {
        return uv_read_start(native_stream(),
                             [](uv_handle_t*, size_t suggested_size, uv_buf_t* buf)
        {
            assert(buf);
            auto size = std::max(suggested_size, max_alloc_size);
            buf->base = new char[size];
            buf->len = size;
        },
        [callback](uv_stream_t* s, ssize_t nread, const uv_buf_t* buf)
        {
            // handle callback throwing exception: hold data in unique_ptr
            std::shared_ptr<char> baseHolder(buf->base, std::default_delete<char[]>());

            if (nread < 0)
            {
                // FIXME error has nread set to -errno, handle failure
                // assert(nread == UV_EOF); ???
                callback(nullptr, nread);
            }
            else if (nread >= 0)
            {
                callback(buf->base, nread);
            }
        }) == 0;
    }

    bool read_stop()
    {
        return uv_read_stop(native_stream()) == 0;
    }

    bool write(const char* buf, int len, CallbackWithResult callback)
    {
        m_write_cb = callback;
        uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(buf), static_cast<size_t>(len) } };
        return uv_write(new uv_write_t, native_stream(), bufs, 1, [](uv_write_t* req, int status)
        {
            std::unique_ptr<uv_write_t> reqHolder(req);
            callback( error(status));
        }) == 0;
    }

    bool write(const std::string& buf, CallbackWithResult callback)
    {
        uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(buf.c_str()), buf.length()} };
        return uv_write(new uv_write_t, native_stream(), bufs, 1, [callback](uv_write_t* req, int status)
        {
            std::unique_ptr<uv_write_t> reqHolder(req);
            callback( error(status));
        }) == 0;
    }

    bool write(const std::vector<char>& buf, CallbackWithResult callback)
    {
        uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(&buf[0]), buf.size() } };
        return uv_write(new uv_write_t, native_stream(), bufs, 1, [callback](uv_write_t* req, int status)
        {
            std::unique_ptr<uv_write_t> reqHolder(req);
            callback( error(status));
        }) == 0;
    }

    bool shutdown(CallbackWithResult callback)
    {
        return uv_shutdown(new uv_shutdown_t, native_stream(), [callback](uv_shutdown_t* req, int status)
        {
            std::unique_ptr<uv_shutdown_t> reqHolder(req);
            callback( error(status));
        }) == 0;
    }
protected:

    CallbackWithResult m_listen_cb;
    CallbackWithResult m_write_cb;
    CallbackWithResult m_shutdown_cb;

};
}
