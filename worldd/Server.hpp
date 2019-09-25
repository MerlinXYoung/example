#pragma once
namespace idiot
{
class IServer
{
public:
    virtual int OnClientConnect()=0;
    virtual int OnClientAuth()=0;
    virtual int OnClinetClose() =0;
    virtual int OnMessage() = 0 ;
public:
    virtual int OnInit() =0;
    virtual int OnLoad() =0;
    virtual int OnFini() =0;

};
}