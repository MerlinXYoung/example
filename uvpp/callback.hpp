#pragma once


#include <functional>

#include <uvpp/error.hpp>

namespace uvpp {

typedef std::function<void()> Callback;
typedef std::function<void(error)> CallbackWithResult;

}

