#pragma once
#include <memory>

#if __cplusplus >= 201402L
#elif __cplusplus >= 201103L
namespace std
{
template<class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}
#else
#error("error c++ std less 11")
#endif

#define NONE_COPYABLE(T) \
T(const T&)=delete; \
T& operator()(const T&)=delete

namespace std{
template<class T>
void bzero(T& t)
{
    bzero(&t, sizeof(t));
}

template<class T>
void bzero(T* p)
{
    bzero(p, sizeof(T));
}
}

