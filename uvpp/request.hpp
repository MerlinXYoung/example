#pragma once

#include <uvpp/callback.hpp>
#include <uvpp/error.hpp>
#include <string.h>

namespace uvpp {


/**
 * Wraps a libuv's uv_handle_t, or derived such as uv_stream_t, uv_tcp_t etc.
 *
 * Resources are released on the close call as mandated by libuv and NOT on the dtor
 */
template<typename REQUEST_T>
class request
{
public:
    typedef request<REQUEST_T> this_type;

    typedef REQUEST_T nvative_type;
    typedef REQUEST_T* native_pointer;
    typedef const REQUEST_T* const_native_pointer;

    typedef uv_req_t native_req_type;
    typedef uv_req_t* native_req_pointer;
    typedef const uv_req_t* const_native_req_pointer;
protected:
    request()
    {
    }

    request(request&& other):
        m_uv_request(other.m_uv_request)
    {
        bzero(&other.m_uv_request, sizeof(other.m_uv_request));
    }

    request& operator=(request&& other)
    {
        if (this == &other)
            return *this;
        m_uv_request = other.m_uv_request;
        bzero(&other.m_uv_request, sizeof(other.m_uv_request));
        return *this;
    }

    ~request()
    {

    }

    request(const request&) = delete;
    request& operator=(const request&) = delete;

public:

    inline native_pointer native()
    {
        return reinterpret_cast<native_pointer>(&m_uv_request);
    }


    inline const_native_pointer native() const
    {
        return reinterpret_cast<const_native_pointer>(&m_uv_request);
    }

    inline native_req_pointer native_req()
    {
        return reinterpret_cast<native_req_pointer>(native());
    }


    inline const_native_req_pointer native_req() const
    {
        return reinterpret_cast<const_native_req_pointer>(native());
    }

    int cancel()
    {
        return uv_cancel(native_req());
    }

protected:
    REQUEST_T m_uv_request;
};

}