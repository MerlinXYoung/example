#include "app_id.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#if 1
appid_t str2appid(const char* str)
{
    struct in_addr netAddr;
    return static_cast<appid_t>(inet_aton(str, &netAddr));

}
const char* appid2str(appid_t id)
{
    struct in_addr netAddr = {id};
    return inet_ntoa(netAddr);
}
#endif

