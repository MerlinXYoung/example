#pragma once

#include <memory>

#include "Server.hpp"
#include <boost/noncopyable.hpp>
class Server final:public IServer , public boost::noncopyable
{
private:
    using impl_ptr = std::unique_ptr<IServer, void(*)(IServer*)>;
public: 
    Server();
    virtual ~Server();

public:
    virtual int Init(const std::string& id) override
    {
        return impl_->Init(id);
    }
    virtual int Load(const std::string& cfg, const std::string& log) override
    {
        return impl_->Load(cfg, log);
    }
    // virtual int OnMessage()=0;
    // virtual int OnNewClient()=0;
    virtual int Fini()override
    {
        return impl_->Fini();
    }
    virtual int Run()override
    {
        return impl_->Run();
    }
public:
    inline void set_impl(IServer* impl){
        impl_.reset( impl);
    }
private:

    impl_ptr impl_;
};