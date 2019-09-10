#pragma once

#include <memory>
#include <sstream>
//#include "../src/cs/config.hpp"


/// Formatted string, allows to use stream operators and returns a std::string with the resulting format
#if __cplusplus >= 201402L
#define fs(x) \
   (static_cast<const std::ostringstream&>(((*std::make_unique<std::ostringstream>().get()) << x)).str ())
#elif __cplusplus >= 201103L
#define fs(x) \
   (static_cast<const std::ostringstream&>(((*std::unique_ptr<std::ostringstream>(new std::ostringstream).get()) << x)).str ())
#else
    #error("please use c++=11")
#endif
namespace uvactor {
typedef sockaddr_in ip4_addr;
typedef sockaddr_in6 ip6_addr;

inline ip4_addr to_ip4_addr(const std::string& ip, int port)
{
    ip4_addr result;
    int res = 0;
    if ((res = uv_ip4_addr(ip.c_str(), port, &result)) != 0)
        throw std::runtime_error(fs("uv_ip4_addr error: " << uv_strerror(res)));
    return result;
}

inline ip6_addr to_ip6_addr(const std::string& ip, int port)
{
    ip6_addr result;
    int res = 0;
    if ((res = uv_ip6_addr(ip.c_str(), port, &result)) != 0)
        throw std::runtime_error(fs("uv_ip6_addr error: " << uv_strerror(res)));
    return result;
}

inline bool from_ip4_addr(ip4_addr* src, std::string& ip, int& port)
{
    char dest[16];
    if (uv_ip4_name(src, dest, 16) == 0)
    {
        ip = dest;
        port = static_cast<int>(ntohs(src->sin_port));
        return true;
    }
    return false;
}

inline bool from_ip6_addr(ip6_addr* src, std::string& ip, int& port)
{
    char dest[46];
    if (uv_ip6_name(src, dest, 46) == 0)
    {
        ip = dest;
        port = static_cast<int>(ntohs(src->sin6_port));
        return true;
    }
    return false;
}
}
#undef fs