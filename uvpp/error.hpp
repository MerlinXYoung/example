#pragma once

#include <assert.h>
#include <stdexcept>
#include <string>
#include <uv.h>

namespace uvpp {


class error
{
public:
    error(int c):
        m_error(c)
    {
    }

public:
    explicit operator bool() const
    {
        return m_error != 0;
    }

    const char* str() const
    {
        return uv_strerror(m_error);
    }

private:
    int m_error;
};

class exception: public std::runtime_error
{
public:
    exception(const std::string& message):
        std::runtime_error(message)
    {}
    error err(){return error(m_error);}
protected:
    int m_error;
};
}

