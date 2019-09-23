#include <uvcurlpp/Multi.h>
#include <assert.h>
#include <uvcurlpp/memory.h>
namespace uvcurlpp
{
static void _check_multi_info(Multi& multi);
static void _curl_perform(uv_poll_t *req, int status, int events);
typedef struct curl_context_s {
    
    uv_poll_t poll_handle;
    curl_socket_t sockfd;
    Multi* multi_;
} curl_context_t;

static curl_context_t *_create_curl_context(Multi* multi, curl_socket_t sockfd) {
    uvcurl_malloc_obj(curl_context_t, context);

    context->sockfd = sockfd;

    int r = uv_poll_init_socket(multi->get_uvloop(), &context->poll_handle, sockfd);
    assert(r == 0);
    context->poll_handle.data = context;
    context->multi_ = multi;

    return context;
}

static void _destroy_curl_context(curl_context_t *context) {
    uv_close((uv_handle_t*) &context->poll_handle, [](uv_handle_t *handle) {
        curl_context_t *context = reinterpret_cast<curl_context_t*>(handle->data);
        free(context);
    });
}

Multi::Multi(uv_loop_t* loop):loop_(loop),curlm_(curl_multi_init(),curl_multi_cleanup)
{
    assert(NULL == loop_);
    assert(curlm_.valid() );
    assert(0 == uv_timer_init(loop_, &timer_));
    timer_.data = this;
    curl_multi_setopt(curlm_.get(), CURLMOPT_SOCKETDATA, this);
    curl_multi_setopt(curlm_.get(), CURLMOPT_SOCKETFUNCTION, [](CURL *easy, curl_socket_t s, int action, void *userp, void *socketp)->int {
        Multi* multi = reinterpret_cast<Multi*>(userp);
        CURLM* curlm = multi->get_curlm();
        curl_context_t *curl_context;
        if (action == CURL_POLL_IN || action == CURL_POLL_OUT) {
            if (socketp) {
                curl_context = (curl_context_t*) socketp;
            }
            else {
                curl_context = _create_curl_context(multi, s);
                curl_multi_assign(curlm, s, (void *) curl_context);
            }
        }

        switch (action) {
            case CURL_POLL_IN:
                uv_poll_start(&curl_context->poll_handle, UV_READABLE, _curl_perform);
                break;
            case CURL_POLL_OUT:
                uv_poll_start(&curl_context->poll_handle, UV_WRITABLE, _curl_perform);
                break;
            case CURL_POLL_REMOVE:
                if (socketp) {
                    uv_poll_stop(&((curl_context_t*)socketp)->poll_handle);
                    _destroy_curl_context((curl_context_t*) socketp);                
                    curl_multi_assign(curlm, s, NULL);
                }
                break;
            default:
                abort();
        }

        return 0;
    });
    curl_multi_setopt(curlm_.get(), CURLMOPT_TIMERFUNCTION, [](CURLM *curlm, long timeout_ms, void *userp) {
        Multi* multi = reinterpret_cast<Multi*>(userp);
        //CURLM* curlm = multi->get_curlm();
        if (timeout_ms <= 0)
            timeout_ms = 1; /* 0 means directly call socket_action, but we'll do it in a bit */
        uv_timer_start(multi->get_timer(), [](uv_timer_t *req) {
            int running_handles;
            Multi* multi = reinterpret_cast<Multi*>(req->data);
            curl_multi_socket_action(multi->get_curlm(), CURL_SOCKET_TIMEOUT, 0, &running_handles);
            _check_multi_info(*multi);
        }, 
        timeout_ms, 0);
    });
}
Multi::~Multi()
{

}

int Multi::async_preform(CURL* handle, done_cb_t done_cb)
{
    done_cb_map_.emplace(handle, done_cb);
    return curl_multi_add_handle(curlm_.get(), handle);
}
void Multi::curl_done(CURL* handle)
{
    auto it = done_cb_map_.find(handle);
    if(it != done_cb_map_.end())
    {
        it->second(it->first);
        done_cb_map_.erase(it);
    }
}
static void _check_multi_info(/*CURLM* curlm*/ Multi& multi) {
    CURLM* curlm = multi.get_curlm();
    char *done_url;
    CURLMsg *message;
    int pending;

    while ((message = curl_multi_info_read(curlm, &pending))) {
        switch (message->msg) {
        case CURLMSG_DONE:
            curl_easy_getinfo(message->easy_handle, CURLINFO_EFFECTIVE_URL,
                            &done_url);
            printf("%s DONE\n", done_url);
            multi.curl_done(message->easy_handle);
            

            curl_multi_remove_handle(curlm, message->easy_handle);
            curl_easy_cleanup(message->easy_handle);
            break;

        default:
            fprintf(stderr, "CURLMSG default\n");
            abort();
        }
    }
}
static void _curl_perform(uv_poll_t *req, int status, int events) {
    curl_context_t *context = reinterpret_cast<curl_context_t*>(req);
    uv_timer_stop(context->multi_->get_timer());
    int running_handles;
    int flags = 0;
    if (status < 0)                      flags = CURL_CSELECT_ERR;
    if (!status && events & UV_READABLE) flags |= CURL_CSELECT_IN;
    if (!status && events & UV_WRITABLE) flags |= CURL_CSELECT_OUT;

    

    curl_multi_socket_action(context->multi_->get_curlm(), context->sockfd, flags, &running_handles);
    _check_multi_info(*context->multi_);   
}

}