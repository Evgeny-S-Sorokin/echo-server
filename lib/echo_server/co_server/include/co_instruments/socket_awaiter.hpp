#pragma once

#include "awaiter.hpp"

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
        auto tryBeforeSuspend = v_socket.IsNextActionReady();
        if (tryBeforeSuspend > 0)
        {
            return tryBeforeSuspend;
        }
        return {};
    }

    void Suspend(std::coroutine_handle<> handle)
    { 
        v_socket.SuspendNextAction(handle); 
    }

    int Resume() 
    { 
        return v_socket.PerformNextAction();
    }

private:
    SocketType& v_socket;
};

} // namespace instruments
} // namespace echo_servers