#pragma once

#include <uvpp/request.hpp>
#include <uvpp/error.hpp>
#include <uvpp/loop.hpp>

namespace uvpp {
class Work : public request<uv_work_t>
{
public:
    Work(loop& l) : request<uv_work_t>(), loop_(l.native())
    {

    }

    bool execute(Callback callback, CallbackWithResult afterCallback)
    {


        callbacks::store(native()->data, internal::uv_cid_work, callback);
        callbacks::store(native()->data, internal::uv_cid_after_work, afterCallback);

        return (
                   uv_queue_work(loop_, native(),
                                 [callback](uv_work_t* req)
        {
            callback();
        },
        [afterCallback](uv_work_t* req, int status)
        {
            afterCallback( error(status));
        }) == 0
               );
    }
private:
    uv_loop_t *loop_;
};
}
