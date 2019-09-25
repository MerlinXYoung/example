#pragma once
#include "Server.hpp"
#include <memory>
namespace idiot
{
class Server final :public IServer
{
public:
    using impl_ptr = std::unique_ptr<IServer> ;
    virtual int OnClientConnect() override;
    virtual int OnClientAuth()override;
    virtual int OnClinetClose() override;
    virtual int OnMessage() override ;
public:
    virtual int OnInit() override;
    virtual int OnLoad() override;
    virtual int OnFini() override;
public:
    void set_impl(impl_ptr&& impl )
    {
        impl_ = std::move(impl);
    }
    void set_impl(impl_ptr& impl )
    {
        std::swap(impl_ ,impl);
    }
private:
    impl_ptr impl_; 
};
}