#pragma once

#include <coroutine>
#include <exception>

namespace echo_servers
{
namespace instruments
{

struct CoListenTask
{
    struct CoListenTaskPromise
    {
        using handleType = std::coroutine_handle<CoListenTaskPromise>;

        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        CoListenTask get_return_object() 
        { 
            return { .v_handle = handleType::from_promise(*this) }; 
        }
        void return_void() {}
    };
    using promise_type = CoListenTaskPromise;

    std::coroutine_handle<> v_handle = nullptr;
};

struct CoEchoTask
{
    struct CoEchoTaskPromise
    {
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        CoEchoTask get_return_object() { return {}; }
        void return_void() {}
    };
    using promise_type = CoEchoTaskPromise;
};

} // namespace instruments
} // namespace echo_servers