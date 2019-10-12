#include "Server.h"

using namespace std;

Server::Server():impl_(CreateServer(), DestroyServer){

}

Server::~Server(){

}

// int Server::Init(const string& id){
    
//     return OnInit(id);

// }

// int Server::Load(const string& cfg, const string& log)
// {
//     return OnLoad(cfg, log);
// }

// int Server::Fini()
// {
//     return OnFini();
// }

// int Server::Run()
// {
//     return OnRun();
// }