#pragma once

#include <coroutine>
#include <iostream>

struct epoll_event;

namespace echo_servers
{
namespace instruments
{

class CoSocketHandle
{
public:
    CoSocketHandle() noexcept = default;
    ~CoSocketHandle() noexcept = default;

    void SuspendNextAction(std::coroutine_handle<> handle)
    {
        v_handle = handle;
    }

    void ResumeNextAction()
    {
        v_handle.resume();
    }

protected:
    std::coroutine_handle<> v_handle = nullptr; // coroutine with info about next action
};

} // namespace instruments
} // namespace echo_servers