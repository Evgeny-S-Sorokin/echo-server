#pragma once

#include "awaiter.hpp"
#include "co_socket.hpp"

#include <coroutine>
#include <stdexcept>

namespace echo_servers
{
namespace instruments
{

template<typename SocketType>
class SocketAwaiter 
    : public EpollAwaiter<SocketAwaiter<SocketType>, int>
{
public:
    SocketAwaiter(SocketType& socket)
        : v_socket(socket)
    {}

    std::optional<int> ShouldSuspend()
    {
        auto tryBeforeSuspend = v_socket.OnEvent();
        if (tryBeforeSuspend > 0 || (errno != EAGAIN && errno != EWOULDBLOCK))
        {
            return tryBeforeSuspend;
        }
        return {};
    }
    void Suspend(std::coroutine_handle<> handle)
    { 
        v_socket.WaitForEvent(handle); 
    }
    int Resume() 
    { 
        return v_socket.OnEvent();
    }

private:
    SocketType& v_socket;
};

} // namespace instruments
} // namespace echo_servers