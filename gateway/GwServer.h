#pragma once

#include "Server.hpp"
#include "handler.hpp"
#include <memory>
#include <vector>
#include <uv.h>
#include "app_id.h"
#include <gateway.pb.h>
#include <uvcurl/Multi.hpp>
#include "Acceptor.h"
#include "cs_gateway.pb.h"

class IAuthFactory;
class GwServer final:public IServer, public IHandler
{
public:
    using loop_delete = void(*)(uv_loop_t*);
    using loop_ptr = std::unique_ptr<uv_loop_t, loop_delete>;
    using curl_multi_ptr = std::unique_ptr<uvcurl::Multi> ;
    using auth_factory_ptr = std::unique_ptr<IAuthFactory> ;
    using acceptor_ptr = std::unique_ptr<Acceptor>;

public:
    explicit GwServer(bool use_default_uv_loop=true);
    virtual ~GwServer();
public:
    // IServer
    virtual int Init(const std::string& id)override;
    virtual int Load(const std::string& cfg, const std::string& log)override;
    // virtual int Message()override;
    // virtual int NewClient()override;
    virtual int Fini()override;
    virtual int Run()override;
public:
    virtual int handle_pkg(Client::pointer client, const Client::pkg_t& pkg)override;

private:  
    int doProcess(Client::pointer client, const gw::cs::Head& head, google::protobuf::Message& msg);
    int doAuth(Client::pointer client, const gw::cs::Head& head, gw::cs::AuthReq& req);
public:
    inline void set_auth_factory(IAuthFactory* factory)
    {
        auth_factory_.reset(factory);
    }
    inline curl_multi_ptr::pointer multi(){return multi_.get();}
private:
    appid_t appid_;
    gw::cfg::Cfg cfg_;
    loop_ptr loop_;
    curl_multi_ptr multi_;
    auth_factory_ptr auth_factory_; 
    std::vector<acceptor_ptr> acceptors_;

};