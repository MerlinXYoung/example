#pragma once
#include <vector>
#include <uv.h>
namespace uvactor
{
class Connection
{
public:
    enum class EStatus
    {
        New = 0,
        Init = 1,
        Start ,
        Connected,
        Restart,
        TimeWait,
    };
    typedef uv_buf_t msg_type; 
    Connection(){}
    virtual ~Connection(){}
    bool Init(uv_loop_t& loop, size_t max_send_bufs_size=200);
    bool Start(const char* ip, int port);

    int Send(msg_type& msg);
public:
    void ConnectCb(int status);
    void TimerCb();
protected:
    bool Restart();
    bool StartTimer();
    bool StopTimer();


protected:
    EStatus status_;
    struct address_in addr_;
    uv_tcp_t tcp_;
    uv_timer_t timer_;
    size_t max_send_bufs_size_;
    std::vector<msg_type> send_bufs_;
    bool writing_;
};
}