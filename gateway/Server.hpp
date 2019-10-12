#pragma once
#include <string>

typedef uint32_t appid_t;
class IServer
{
public:
    
    virtual ~IServer(){}
    virtual int Init(const std::string& id)=0;
    virtual int Load(const std::string& cfg, const std::string& log)=0;
    // virtual int OnMessage()=0;
    // virtual int OnNewClient()=0;
    virtual int Fini()=0;
    virtual int Run()=0;
};

IServer* CreateServer();
void DestroyServer(IServer*);