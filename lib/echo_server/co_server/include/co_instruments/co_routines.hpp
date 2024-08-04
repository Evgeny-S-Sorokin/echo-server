#pragma once

#include "co_server/co_listener_socket.hpp"
#include <bits/utility.h>
#include <coroutine>
#include <exception>
#include <iostream>
#include <new>
#include <tuple>

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

        void operator delete(void *ptr)
        {
            if (ptr)
            {
                std::cout << "CoListenTask deleted...\n";
                free(ptr);
            }
        }

        template<typename... Args>
        void *operator new(std::size_t size, Args &&...args)
        {
            auto buffer = std::malloc(size);
            std::cout << "CoListenTask allocated...\n";
            return buffer;
        }
    };
    using promise_type = CoListenTaskPromise;

    ~CoListenTask() noexcept
    {
        v_handle.destroy();
    }

    std::coroutine_handle<> v_handle = nullptr;
};

struct CoEchoTask
{
    struct CoEchoTaskPromise
    {
        using handle_type = std::coroutine_handle<CoEchoTaskPromise>;
    
        struct initial_suspend : public std::suspend_always
        {};

        struct final_suspend
        {
            bool await_ready() const noexcept { return false; }

            void await_suspend(handle_type endingHandle) noexcept 
            {
                endingHandle.promise().v_awaitingHandle.resume();
            }

            void await_resume() noexcept {}
        };

        initial_suspend initial_suspend() noexcept { return {}; }
        final_suspend final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        CoEchoTask get_return_object() 
        { 
            return CoEchoTask(handle_type::from_promise(*this)); 
        }
        void return_void() {}

        void operator delete(void *ptr)
        {
            if (ptr)
            {
                std::cout << "CoEchoTask deleted...\n";
                free(ptr);
            }
        }

        template<typename... Args>
        void *operator new(std::size_t size, Args &&...args)
        {
            auto buffer = std::malloc(size);
            std::cout << "CoEchoTask allocated...\n";
            return buffer;
        }

        std::coroutine_handle<> v_awaitingHandle = nullptr;
    };
    using promise_type = CoEchoTaskPromise;
    using handle_type = CoEchoTaskPromise::handle_type;

    explicit CoEchoTask(handle_type handle)
        : v_selfHandle(handle)
    {
        std::cout << "Echo task initialized...\n";
    }
    ~CoEchoTask() noexcept
    {
        if (v_selfHandle)
        {
            v_selfHandle.destroy();
        }
    }

    bool await_ready()
    {
        return !v_selfHandle || v_selfHandle.done();
    }

    void await_suspend(std::coroutine_handle<> listenCoroutine) noexcept
    {
        v_selfHandle.resume();
        v_selfHandle.promise().v_awaitingHandle = listenCoroutine;
    }

    void await_resume() const noexcept {}

private:
    handle_type v_selfHandle = nullptr;    
};

} // namespace instruments
} // namespace echo_servers