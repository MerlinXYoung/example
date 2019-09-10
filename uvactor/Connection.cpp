#include <Connection.h>

namespace uvactor
{
bool Connection::Init(uv_loop_t& loop, size_t max_send_bufs_size)
{
    max_send_bufs_size_ = max_send_bufs_size;
    int ires = uv_tcp_init(&loop, &tcp_) ;
    if(ires !=0 )
        return false;
    tcp_.data=this;

    ires = uv_timer_init(&loop, &timer_);
    if(ires != 0)
        return false;
    timer_.data=this;
    status_ = EStatus::Init;
    return true;
}
bool Connection::Start(const char* ip, int prot)
{
    uv_ip4_addr(ip, port, &addr_);
    uv_tcp_connect(new uv_connect_t, &tcp_, &addr, [](uv_connect_t* req, int status){
        auto conn = reinterpret_cast<Connection*>(req->handle->data);
        conn->ConnectCb(int status);

    });
    status_ = EStatus::Start;
    return true;
}



bool Connection::Restart()
{
    uv_tcp_connect(new uv_connect_t, &tcp_, &addr, [](uv_connect_t* req, int status){
        auto conn = reinterpret_cast<Connection*>(req->handle->data);
        conn->ConnectCb(int status);

    });
    status_ = EStatus::Restart;
    return true;
}

void Connection::ConnectCb(int status)
{
    if(0 == status)
    {
        //uv_timer_stop(uv_timer_t* handle);
        switch(status_)
        {
        case EStatus::TimeWait:
            StopTimer();
            break;
        default:
            break;
        }
        status_ = EStatus::Connected;
    }
    else
    {
        StartTimer();
    }
    
}

bool Connection::StartTimer()
{
    status_ = EStatus::TimeWait;
    uv_timer_start(&timer_,
        [](uv_timer_t* handle){
            auto conn = reinterpret_cast<Connection*>(handle->data);
            conn->TimerCb();
        },
        uint64_t 50,
        uint64_t 0);
}

void Connection::TimerCb()
{
    Restart();
}

int Connection::Send(msg_type& msg)
{
    if(status_ == EStatus::Connected)
    {
        if(writing_)
        {
            if(send_bufs_.size()< max_send_bufs_size_)
                send_bufs_.push_back(msg);
            else
                return -1;
        }
        uv_buf_t bufs={{msg}};
        uv_write(new uv_write_t, &tcp_, bufs, 1,
            [](uv_write_t* req, int status){
                
        });
    }
    else
    {
        if(send_bufs_.size()< max_send_bufs_size_)
            send_bufs_.push_back(msg);
        else
            return -1;
    }
    msg.base=nullptr;
    msg.len = 0;
    return 0;
}

}