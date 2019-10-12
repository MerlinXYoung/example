#include "GwServer.h"
#include <pb2json.h>
#include "Acceptor.h"
#include "BackendMgr.h"
#include <curl/curl.h>
#include "QQAuth.h"
#include "stdex.h"
#include "cs_gateway_utility.h"
#include "ClientMgr.h"
#include "log.h"

using namespace std;
// #include <gateway.pb.h>
static uv_loop_t* _create_loop()
{
    auto l = new uv_loop_t;
    uv_loop_init(l);
    return l;
}
static GwServer::loop_ptr _create_loop(bool use_default)
{
    if(use_default)
    {
        return GwServer::loop_ptr(uv_default_loop(),[](uv_loop_t* loop){});
    }
    else
    {
        auto l = new uv_loop_t;
        uv_loop_init(l);
        return GwServer::loop_ptr(l,[](uv_loop_t* loop){
            uv_loop_close(loop);
            delete loop;
        });
    }
}
GwServer::GwServer(bool use_default_uv_loop):appid_(0),loop_(move(_create_loop(use_default_uv_loop))),//loop_(nullptr,[](uv_loop_t* loop){})
    multi_(move(make_unique<uvcurl::Multi>(loop_.get())))
{
    // if(use_default_uv_loop)
    // {
    //     loop_ = move(loop_ptr(uv_default_loop(),[](uv_loop_t* loop){}));
    // }
    // else
    // {
    //     auto l = new uv_loop_t;
    //     uv_loop_init(l);
    //     loop_ = move(loop_ptr(l,[](uv_loop_t* loop){
    //         uv_loop_close(loop);
    //         delete loop;
    //     }));
    // }
    //multi_ = make_unique<uvcurl::Multi>(loop_.get());
    if (curl_global_init(CURL_GLOBAL_ALL)) {
        throw runtime_error("Could not init cURL");
    }
}

GwServer::~GwServer()
{

}

int GwServer::Init(const string& id)
{
    appid_ = str2appid(id.c_str());

    return 0;
}
int GwServer::Load(const string& cfg, const string& log)
{
    if(!json_file2pb(cfg_, cfg))
        return -1;
    
    switch(cfg_.listen().protocol())
    {
    case gw::cfg::Listen_EProtocol::Listen_EProtocol_TCP:
    {
        uint32_t id=343443443;
        acceptors_.emplace_back(new Acceptor);
        auto& acceptor = *acceptors_.rbegin();
        acceptor->init(*loop_.get());
        acceptor->listen(cfg_.listen().ip().c_str(), cfg_.listen().port());
        
    }
    default:
        break;
    }
    for(int i=0;i<cfg_.backends_size();++i)
    {
        BackendMgr::get_mutable_instance().Alloc(cfg_.backends(i).id(), cfg_.backends(i).uri());
    }
    return 0;
}
// int GwServer::OnMessage()
// {
//     return 0;
// }
// int GwServer::OnNewClient()
// {
//     return 0;
// }
int GwServer::Fini()
{
    return 0;
}

int GwServer::Run()
{
    uv_prepare_t prepare_zmq;
    uv_prepare_init(loop_.get(), &prepare_zmq);
    uv_prepare_start(&prepare_zmq, [](uv_prepare_t* handle){
        BackendMgr::get_mutable_instance().recv();
    });
    return uv_run(loop_.get(), UV_RUN_DEFAULT);
}

int GwServer::handle_pkg(Client::pointer client, const Client::pkg_t& pkg)
{
    try{
    do{
      gw::cs::ReqParser parser;
      if(!parser.parse(pkg))
      {
        break;
      }
      if(parser.head()->msgid() == gw::cs::EMsgID::Other)
      {
        if(!client->is_authed())
        {
          // client->async_close();
          ClientMgr::get_mutable_instance().Free(client->id());
          return 0;
        }
        log_trace("body[%.*s],",parser.data().len, parser.data().data );
        auto backend = BackendMgr::get_mutable_instance()
            .Get(client->backend_id());
          if(backend)
            backend->send_client_other(*client, parser.data().data, parser.data().len);
      }
    
      return doProcess(client, *parser.head(), *parser.body());

    }while(0);
    return -1;
  }catch(std::exception & e)
  {
    log_error( "exception:%s", e.what());
  }
  catch(...)
  {

  }
}

int GwServer::doProcess(Client::pointer client, const gw::cs::Head& head, google::protobuf::Message& msg)
{
    switch(head.msgid())
    {
    case gw::cs::EMsgID::Auth:
    {
        return doAuth(client, head, static_cast<gw::cs::AuthReq&>(msg));   
    }
    default:
        log_error( "fuck msgid[%d]", head.msgid());
    }
}
int GwServer::doAuth(Client::pointer client, const gw::cs::Head& head, gw::cs::AuthReq& req)
{
    std::unique_ptr<IAuth> auth(auth_factory_->create());
    return auth->do_auth(client->id(), req.openid().c_str(), req.openkey().c_str());

}

IServer* CreateServer(){
    auto svr = new GwServer;
    if(!svr)
        return nullptr;
    auto factory = new QQAuthFactory;
    if(!factory)
        return nullptr;
    factory->set_multi(svr->multi());
    svr->set_auth_factory(factory);
    return svr;
}
void DestroyServer(IServer* svr)
{
    delete svr;
}