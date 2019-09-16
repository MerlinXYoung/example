#pragma once
#include <stdint.h>
typedef uint32_t appid_t;

appid_t str2appid(const char*);
const char* appid2str(appid_t);