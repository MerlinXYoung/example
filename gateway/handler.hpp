#pragma once

#include "Client.h"
struct IHandler{
    virtual ~IHandler(){}
    virtual int handle_pkg(Client::pointer client, const Client::pkg_t& pkg)=0;
};