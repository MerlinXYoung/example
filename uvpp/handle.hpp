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
template<typename HANDLE_T>
class handle:protected HANDLE_T
{
public:
    typedef HANDLE_T native_type;
    typedef HANDLE_T* native_pointer;
    typedef const HANDLE_T* const_native_pointer;
    typedef handle<HANDLE_T> this_type;
    typedef handle<HANDLE_T>* pointer;
    typedef const handle<HANDLE_T>* const_ponter;
    typedef handle<HANDLE_T>& reference;
    typedef const handle<HANDLE_T>& const_reference;
    typedef uv_handle_t native_handle_type;
    typedef uv_handle_t* native_handle_pointer;
    typedef const uv_handle_t* const_native_handle_pointer;
protected:
    handle()
    {
    }

    handle(handle&& other)
    {
        bzero(native_handle(), sizeof(native_type)) ;
    }

    handle& operator=(handle&& other)
    {
        if (this == &other)
            return *this;

        bzero(native_handle(), sizeof(native_type)) ;
        return *this;
    }

    virtual ~handle()
    {

    }

    handle(const handle&) = delete;
    handle& operator=(const handle&) = delete;

public:
    void set_data(void* _data)
    {
        data = _data;
    }
    template<typename T_>
    T_* data()
    {
        return reinterpret_cast<T_*>(data);
    }

    template<typename T_>
    const T_* data()const 
    {
        return reinterpret_cast<const T_*>(data);
    }
    inline native_pointer native()
    {
        reinterpret_cast<native_pointer>(this);
    }

    inline const_native_pointer native() const
    {
        reinterpret_cast<const_native_pointer>(this);
    }

    inline native_handle_pointer native_handle()
    {
        return reinterpret_cast<native_handle_pointer>(this);
    }

    inline const_native_handle_pointer native_handle()const
    {
        return reinterpret_cast<const_native_handle_pointer>(this);
    }

    bool is_active() const
    {
        return uv_is_active(native_handle()) != 0;
    }


    void close(Callback callback)
    {
        if (uv_is_closing(native_handle()))
        {
            return; // prevent assertion on double close
        }
        m_close_cb = callback;
        uv_close(native_handle(), [](native_handle_pointer h ){
            reinterpret_cast<pointer>(h)->m_close_cb();
        });
    }

    inline int native_fd()const
    {
        return native_handle()->u.fd;
    }


protected:
    Callback m_close_cb;

};

}

