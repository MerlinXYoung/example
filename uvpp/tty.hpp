#pragma once

#include <uvpp/signal.hpp>
#include <uvpp/error.hpp>

namespace uvpp {
class TTY : public stream<uv_tty_t>
{
public:

    enum Type
    {
        STDIN,
        STDOUT,
        STDERR
    };

    TTY(Type type, bool readable):
        stream<uv_tty_t>(), type_(type)
    {
        uv_tty_init(uv_default_loop(), native(), static_cast<int>(type_), static_cast<int>(readable));
    }

    TTY(loop& l, Type type, bool readable):
        stream<uv_tty_t>(), type_(type)
    {
        uv_tty_init(l.native(), native(), static_cast<int>(type_), static_cast<int>(readable));
    }

    error set_mode(uv_tt_mode_t mode)
    {
        return error(uv_tty_set_mode(native(), mode));
    }

    static error reset_mode()
    {
        return error(uv_tty_reset_mode());
    }

    error get_winsize(int& width, int& height)
    {
        return error(uv_tty_get_winsize(native(), &width, &height));
    }
private:
    Type type_;
};
}