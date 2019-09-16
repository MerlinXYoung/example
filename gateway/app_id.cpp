#include "app_id.h"
#include <arpa/inet.h>
#if 0
appid_t str2appid(const char* str)
{
    struct in_addr netAddr;
    netAddr.s_addr = inet_addr(str);
    int ret = inet_aton(strAddr, &netAddr);
    return netAddr.s_addr;

}
const char* appid2str(appid_t)
{
    struct in_addr netAddr;
    netAddr.s_addr = inet_addr(str);
    return inet_ntoa(netAddr);
}
#endif

