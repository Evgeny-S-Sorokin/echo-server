#pragma once

#include "instruments/socket.hpp"
#include "socket_awaiter.hpp"

#include <coroutine>

struct epoll_event;

namespace echo_servers
{
namespace instruments
{

class CoSocket : protected Socket
{
public:
    CoSocket(int fd) noexcept 
        : Socket(fd)
    {}

    void WaitForEvent(std::coroutine_handle<> handle)
    { 
        v_handle = handle;
    }

    void Resume()
    {
        v_handle.resume();
    }

    virtual int OnEvent() = 0;
    virtual void fillEpollEvent(epoll_event* newEvent) noexcept = 0;

protected:
    std::coroutine_handle<> v_handle;
};

} // namespace instruments
} // namespace echo_servers