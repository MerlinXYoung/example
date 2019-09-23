#pragma once

#define uvcurlpp_malloc(T) \
reinterpret_cast<T*>(malloc(sizeof(T)))

#define uvcurlp_malloc_obj(T, obj) \
T* obj =  uvcurlpp_malloc(T)
